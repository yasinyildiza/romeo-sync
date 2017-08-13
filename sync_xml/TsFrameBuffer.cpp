#include <iostream>

#include "TsFrameBuffer.h"
#include "debugMessagePrinter.h"

using namespace std;

TsFrameBufferBase::TsFrameBufferBase()
{
	source = ES_SOURCE_DVB;
	type = ES_VIDEO;
	subtype = ES_BASE;
	index = 0;
	pts_per_frame = PTS_PER_FRAME_VIDEO;
	frame_per_second = FRAME_PER_SECOND_VIDEO;
	sync_PTS = 0;
	sync_status = buffer_not_synced;
	loop_on = false;
}

void TsFrameBufferBase::initializeTs(es_source_types _source, int _index, es_types _type, es_subtypes _subtype, int _pts_per_frame, int _frame_per_second)
{
	source = _source;
	index = _index;
	type = _type;
	subtype = _subtype;
	pts_per_frame = _pts_per_frame;
	frame_per_second = _frame_per_second;
	initialize("f" + to_str(source) + to_str(index) + to_str(type) + to_str(subtype), buffer_size_as_seconds * frame_per_second, buffer_lower_threshold_percentage, buffer_upper_threshold_percentage);
}

const bool TsFrameBufferBase::PTSexists(unsigned int PTS)
{
	BufferNode<TsFrameBase> *current;
	for(current = first; current != NULL; current = current->next)
	{
		if(current->packet->PTS == PTS || ((PTS - current->packet->PTS) / pts_per_frame) == 0)
			return true;
		if(current->packet->PTS > PTS)
			return false;
	}
	return false;
}

void TsFrameBufferBase::dropUntil(unsigned int PTS)
{
	if(isEmpty())
	{
		usleep(2 * USECOND_PER_FRAME);
		dropUntil(PTS);
		return;
	}
	
	while(getFirstPTS() < PTS)
	{
		dropFirst();
		if(isEmpty())
		{
			usleep(2 * USECOND_PER_FRAME);
			dropUntil(PTS);
			return;
		}
	}
}

const unsigned int TsFrameBufferBase::getFirstPTS()
{
	return isEmpty() ? 0 : first->packet->PTS;
}

bool TsFrameBufferBase::resyncByPTS(unsigned int PTS)
{
	if(PTSexists(PTS))
	{
		dropUntil(PTS);
		return true;
	}
	return false;
}

void TsFrameBufferBase::syncByPTS(unsigned int PTS)
{
	int frame_difference = (PTS - getFirstPTS()) / pts_per_frame;
	unsigned int time_difference = (unsigned int)frame_difference / (unsigned int)frame_per_second;

	printDebugMessage("synchronizing buffer " + buffer_id + " " + to_str(frame_difference) + " frames " + to_str(time_difference) + " seconds", STATUS);

	if(isEmpty())
	{
		/*if(waitForNewPacket(sync_timeout_as_seconds))
			syncByPTS(PTS);*/
		printDebugMessage("buffer " + buffer_id + " is empty", STATUS);
		sync_status = buffer_cannot_be_synced;
	}
	/*else if(time_difference >= (unsigned int)buffer_size_as_seconds / 4)
	{
		printDebugMessage("time difference is too long to sync " + buffer_id, WARNING);
		sync_status = buffer_cannot_be_synced;
	}*/
	else
	{
		sync_PTS = PTS;
		sync_status = buffer_syncing_waiting_for_pts;
		dropUntil(PTS);
		sync_status = buffer_synced;
		printDebugMessage("buffer " + buffer_id + " synced", STATUS);
	}
}

void TsFrameBufferBase::insertNewNode(TsFrameBase *packet)
{
	if(sync_status == buffer_cannot_be_synced)
	{
		insertFailed(packet);
		return;
	}
	
	if(sync_status == buffer_syncing_waiting_for_pts)
	{
		if(packet->PTS < sync_PTS)
		{
			insertFailed(packet);
			return;
		}
		else if(packet->PTS == sync_PTS)
		{
			sync_status = buffer_syncing_pts_received;
		}
		else
		{
			printDebugMessage("greater than the expected PTS " + buffer_id, STATUS);
			sync_status = buffer_syncing_pts_received;
		}
	}

	unsigned int first_PTS = getFirstPTS();
	unsigned int received_PTS = packet->PTS;
	unsigned int expected_PTS = last->packet->PTS + pts_per_frame;
	int frame_difference = ((int)received_PTS - (int)expected_PTS) / (int)pts_per_frame;
	if(frame_difference == 0)
	{
		packet->PTS = expected_PTS;
		received_PTS = packet->PTS;
	}

	if(received_PTS > expected_PTS)
	{
		printDebugMessage("buffer " + buffer_id + " missed " + to_str(frame_difference) + " frames in insert", WARNING);
		insertNullFrame(frame_difference, expected_PTS);
	}

	writeDebugMessage(buffer_id + "_insert.txt", to_str(received_PTS) + "\t" + now_str() + " " + getStatusStr() + " " + to_str(getFillRatio()) + "%" + " " + to_str(current_size) + "/" + to_str(total_size), STATUS);

	if(received_PTS >= expected_PTS)
	{
		insertTail(packet);
		packet->setArrivalTime();
	}
	else
	{
		if(received_PTS < first_PTS)
		{
			/*if(frame_difference + FRAME_NUMBER_FOR_LOOP_DETECTION < 0)
			{
				loop_on = true;
				printDebugMessage("buffer " + buffer_id + " loop at " + now_str(), WARNING);
				insertTail(packet);
				packet->setArrivalTime();
			}
			else
			{*/
				printDebugMessage("buffer " + buffer_id + " very early PTS", WARNING);
				insertFailed(packet);
			//}
		}
		else
		{
			lock();
			printDebugMessage("buffer " + buffer_id + " early PTS", WARNING);
			BufferNode<TsFrameBase> *current;
			BufferNode<TsFrameBase> *prev = first;
			for(current = first; current != NULL; current = current->next)
			{
				if(current->packet->PTS < packet->PTS)
				{
					prev = current;
				}
				else if(current->packet->PTS == packet->PTS)
				{
					printDebugMessage("buffer " + buffer_id + " existing PTS", WARNING);
					insertFailed(packet);
					return;
				}
				else
				{
					break;
				}
			}
			prev->insertAfter(new BufferNode<TsFrameBase>(packet));
			insertSuccessful(packet);
			packet->setArrivalTime();
		}
	}
}

TsFrameBufferBase::~TsFrameBufferBase(){}

TsVideoFrameBuffer::TsVideoFrameBuffer(es_source_types _source, int _index, es_subtypes _subtype)
{
	initializeTs(_source, _index, ES_VIDEO, _subtype, PTS_PER_FRAME_VIDEO, FRAME_PER_SECOND_VIDEO);
}

void TsVideoFrameBuffer::insertNullFrame(int number_of_missed_frames, unsigned int PTS)
{
	return;
	TsVideoFrame *null_frame;
	for(int i=0; i<number_of_missed_frames; i++)
	{
		null_frame = new TsVideoFrame();
		null_frame->PTS = PTS + pts_per_frame * i;
		writeDebugMessage(buffer_id + "_insert.txt", to_str(null_frame->PTS) + "\t" + now_str() + " NULL " + to_str(i), STATUS);
		insertTail(null_frame);
	}
}

TsFrameBase *TsVideoFrameBuffer::getFrameByPTS(unsigned int PTS)
{
	writeDebugMessage(buffer_id + "_request.txt", to_str(PTS) + "\t" + now_str(), INFO);
	/*if(sync_status == buffer_cannot_be_synced || isEmpty() || isFull())
		return NULL;*/
	
	string error_string = "buffer " + buffer_id + " cannot send " + to_str(PTS) + " because it is ";
	if(sync_status == buffer_cannot_be_synced)
	{
		printDebugMessage(error_string + "not synced", BEYOND);
		return NULL;
	}
	else if(isEmpty())
	{
		printDebugMessage(error_string + "empty", INFO);
		return NULL;
	}
	else if(isFull())
	{
		printDebugMessage(error_string + "full", INFO);
		//return NULL;
	}
	
	int frame_difference = (getFirstPTS() - PTS) / pts_per_frame;
	
	
	if(getFirstPTS() == PTS || frame_difference == 0)
	{
		writeDebugMessage(buffer_id + "_send.txt", to_str(first->packet->PTS) + "\t" + now_str(), INFO);
		return first->packet;
	}

	if(getFirstPTS() < PTS)
	{
		if(frame_difference > FRAME_NUMBER_FOR_LOOP_DETECTION)
		{
			return NULL;
		}
		else
		{
			printDebugMessage("buffer " + buffer_id + " resync", INFO);
			dropUntil(PTS);
			return getFrameByPTS(PTS);
		}
	}
	
	printDebugMessage("buffer " + buffer_id + " missed " + to_str(frame_difference) + " frames in send", WARNING);
	return NULL;
}

TsVideoFrameBuffer::~TsVideoFrameBuffer(){}

TsAudioFrameBuffer::TsAudioFrameBuffer(es_source_types _source, int _index, es_subtypes _subtype)
{
	initializeTs(_source, _index, ES_AUDIO, _subtype, PTS_PER_FRAME_AUDIO, FRAME_PER_SECOND_AUDIO);
}

void TsAudioFrameBuffer::insertNullFrame(int number_of_missed_frames, unsigned int PTS)
{
	TsAudioFrame *null_frame;
	for(int i=0; i<number_of_missed_frames; i++)
	{
		null_frame = new TsAudioFrame();
		null_frame->PTS = PTS + pts_per_frame * i;
		writeDebugMessage(buffer_id + "_insert.txt", to_str(null_frame->PTS) + "\t" + now_str() + " NULL " + to_str(i), STATUS);
		insertTail(null_frame);
	}
}

TsFrameBase *TsAudioFrameBuffer::getFrameByPTS(unsigned int PTS)
{
	if(sync_status == buffer_cannot_be_synced || isEmpty() || isFull())
	{
		return NULL;
	}
	
	if((getFirstPTS() < PTS + PTS_PER_FRAME_VIDEO / 2) && (getFirstPTS() > PTS - PTS_PER_FRAME_VIDEO / 2))
	{
		writeDebugMessage(buffer_id + "_send.txt", to_str(first->packet->PTS) + "\t" + now_str(), INFO);
		return first->packet;
	}

	int frame_difference = (getFirstPTS() - PTS) / pts_per_frame;

	if(getFirstPTS() < PTS)
	{
		if(frame_difference > FRAME_NUMBER_FOR_LOOP_DETECTION)
		{
			return NULL;
		}
		else
		{
			printDebugMessage("buffer " + buffer_id + " resync", INFO);
			dropUntil(PTS);
			return getFrameByPTS(PTS);
		}
	}

	return NULL;
}

TsAudioFrameBuffer::~TsAudioFrameBuffer(){}

TsPrivateDataFrameBuffer::TsPrivateDataFrameBuffer(es_source_types _source, int _index, es_subtypes _subtype)
{
	initializeTs(_source, _index, ES_PRIVATE_DATA, _subtype, PTS_PER_FRAME_PRIVATE_DATA, FRAME_PER_SECOND_PRIVATE_DATA);
}

void TsPrivateDataFrameBuffer::syncByPTS(unsigned int PTS)
{
	int frame_difference = (PTS - getFirstPTS()) / pts_per_frame;
	double time_difference = (double)frame_difference / (double)frame_per_second;

	printDebugMessage("synchronizing buffer " + buffer_id + " " + to_str(frame_difference) + " frames " + to_str(time_difference) + " seconds", STATUS);

	if(isEmpty())
	{
		printDebugMessage("buffer " + buffer_id + " is empty", STATUS);
		sync_status = buffer_cannot_be_synced;
	}
	/*else if(time_difference >= buffer_size_as_seconds / 4)
	{
		printDebugMessage("time difference is too long to sync " + buffer_id, WARNING);
		sync_status = buffer_cannot_be_synced;
	}*/
	else
	{
		sync_PTS = PTS;
		sync_status = buffer_synced;
		printDebugMessage("buffer " + buffer_id + " synced", STATUS);
	}
}

void TsPrivateDataFrameBuffer::insertNewNode(TsFrameBase *packet)
{
	unsigned int received_PTS = packet->PTS;
	insertTail(packet);
	packet->setArrivalTime();
	writeDebugMessage(buffer_id + "_insert.txt", to_str(received_PTS) + "\t" + now_str() + " " + getStatusStr() + " " + to_str(getFillRatio()) + "%" + " " + to_str(current_size) + "/" + to_str(total_size), STATUS);
}

void TsPrivateDataFrameBuffer::insertNullFrame(int number_of_missed_frames, unsigned int PTS)
{
	TsPrivateDataFrame *null_frame;
	for(int i=0; i<number_of_missed_frames; i++)
	{
		null_frame = new TsPrivateDataFrame();
		null_frame->PTS = PTS + pts_per_frame * i;
		writeDebugMessage(buffer_id + "_insert.txt", to_str(null_frame->PTS) + "\t" + now_str() + " NULL " + to_str(i), STATUS);
		insertTail(null_frame);
	}
}

TsFrameBase *TsPrivateDataFrameBuffer::getFrameByPTS(unsigned int PTS)
{
	if(sync_status == buffer_cannot_be_synced || isEmpty() || isFull())
	{
		return NULL;
	}
	
	writeDebugMessage(buffer_id + "_send.txt", to_str(first->packet->PTS) + "\t" + now_str() + " " + getStatusStr() + " " + to_str(getFillRatio()) + "%" + " " + to_str(current_size) + "/" + to_str(total_size), INFO);
	return first->packet;
	
	/*if((getFirstPTS() < PTS + PTS_PER_FRAME_VIDEO / 2) && (getFirstPTS() > PTS - PTS_PER_FRAME_VIDEO / 2))
	{
		
		return first->packet;
	}

	int frame_difference = (getFirstPTS() - PTS) / pts_per_frame;

	if(getFirstPTS() < PTS)
	{
		if(frame_difference > FRAME_NUMBER_FOR_LOOP_DETECTION)
		{
			return NULL;
		}
		else
		{
			printDebugMessage("buffer " + buffer_id + " resync", INFO);
			dropUntil(PTS);
			return getFrameByPTS(PTS);
		}
	}

	return NULL;*/
}

TsPrivateDataFrameBuffer::~TsPrivateDataFrameBuffer(){}
