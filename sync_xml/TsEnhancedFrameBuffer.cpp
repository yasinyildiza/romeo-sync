#include <iostream>
#include <stdlib.h>

#include "TsEnhancedFrameBuffer.h"
#include "debugMessagePrinter.h"

using namespace std;

TsEnhancedFrameBufferBase::TsEnhancedFrameBufferBase()
{
	source = ES_SOURCE_DVB;
	index = 0;
	type = ES_VIDEO;
	pts_per_frame = PTS_PER_FRAME_VIDEO;
	frame_per_second = FRAME_PER_SECOND_VIDEO;
	decoder_protocol = "TCP";
	decoder_ip = "localhost";
	decoder_port = 0;
	loop_on = false;
	sender = NULL;
	frame_counter = 0;
	is_primary_stream = true;
}

void TsEnhancedFrameBufferBase::initializeTs(es_source_types _source, int _index, es_types _type, int _pts_per_frame, int _frame_per_second)
{
	source = _source;
	index = _index;
	type = _type;
	pts_per_frame = _pts_per_frame;
	frame_per_second = _frame_per_second;
	if(source + index < number_of_primary_streams)
		is_primary_stream = true;
	else
		is_primary_stream = false;
	initialize("ef" + to_str(source) + to_str(index) + to_str(type), buffer_size_as_seconds * frame_per_second, buffer_lower_threshold_percentage, buffer_upper_threshold_percentage);
}

void TsEnhancedFrameBufferBase::initSender(std::string _decoder_protocol, std::string _decoder_ip, int _decoder_port_base)
{
	decoder_protocol = _decoder_protocol;
	decoder_ip = output_for_vlc_flag ? vlc_player_ip : _decoder_ip;
	decoder_port = _decoder_port_base + source + index;
	sender = createClient(decoder_protocol, decoder_ip, decoder_port, true);
	
	system(("mkdir -p log/" + decoder_ip + "_" + to_str(decoder_port)).c_str());
	
	printDebugMessage("sender: " + decoder_protocol + " " + decoder_ip + ":" + to_str(decoder_port), INFO);
}

const unsigned int TsEnhancedFrameBufferBase::getFirstPTS()
{
	return isEmpty() ? 0 : first->packet->PTS;
}

void TsEnhancedFrameBufferBase::dropUntil(unsigned int PTS)
{
	cout << buffer_id + " drop until --> first PTS: " << getFirstPTS() << " PTS: " << PTS << endl;
	
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

void TsEnhancedFrameBufferBase::insertNewNode(TsEnhancedFrame *packet)
{
	//unsigned int first_PTS = first->packet->PTS;
	unsigned int received_PTS = packet->PTS;
	unsigned int expected_PTS = last->packet->PTS + pts_per_frame;
	int frame_difference = ((int)received_PTS - (int)expected_PTS) / (int)pts_per_frame;

	//writeDebugMessage(buffer_id + "_insert.txt", to_str(received_PTS) + "\t" + now_str(), STATUS);
	writeDebugMessage(buffer_id + "_insert.txt", to_str(received_PTS) + "\t" + now_str() + " " + getStatusStr() + " " + to_str(getFillRatio()) + "%" + " " + to_str(current_size) + "/" + to_str(total_size), STATUS);

	if(received_PTS > expected_PTS)
		printDebugMessage("buffer " + buffer_id + " missed " + to_str(frame_difference) + " frames in insert", WARNING);
	
	/*if((received_PTS < first_PTS) && !loop_on)
	{
		printDebugMessage("buffer " + buffer_id + " loop at " + now_str() + " insert", WARNING);
		loop_on = true;
	}*/

	insertTail(packet);
}

void TsEnhancedFrameBufferBase::checkLoop(unsigned int *PTS)
{
	/*if(loop_on)
	{
		printDebugMessage("buffer " + buffer_id + " loop at " + now_str() + " send", WARNING);
		loop_on = false;
		*PTS = getFirstPTS();
		sendFrameByPTS(PTS);
	}*/
}

void TsEnhancedFrameBufferBase::sendOK(TsEnhancedFrame *frame)
{
	if(decoder_protocol == "TCP" && type != ES_PRIVATE_DATA)
	{
		sender->send(frame->size_str, 4);
		if(write_output_to_file_flag)
			writeRawData(to_str(decoder_port) + "/" + to_str(decoder_port) + "_size", frame->size_str, 4);
	}
	if(output_for_vlc_flag)
	{
		for(int i=0; i<(frame->size / TS_PACKET_SIZE); i++)
		{
			sender->send(frame->data+(i*TS_PACKET_SIZE), TS_PACKET_SIZE);
			usleep(1000);
		}
	}
	else
	{
		sender->send(frame->data, frame->size);
	}
	
	if(frame_counter == 0)
	{
		printDebugMessage(buffer_id + " first PTS to send: " + to_str(frame->PTS) + "\t" + now_str(), INFO);
	}
	if(write_output_to_file_flag)
	{
		writeRawData(decoder_ip + "_" + to_str(decoder_port) + "/" + to_str(decoder_port) + "_" + to_str(frame_counter), frame->data, frame->size);
		writeRawData(decoder_ip + "_" + to_str(decoder_port) + "/" + to_str(decoder_port), frame->data, frame->size);
	}
	//if(decoder_ip == audio_secondary_decoder_ip_base || decoder_ip == private_data_decoder_ip_base)
		printDebugMessage(to_str(frame->PTS) + "\t" + now_str()+ "\t" + to_str(frame->size) + "\t" + decoder_ip + ":" + to_str(decoder_port) + "\t", LOOP);
	dropFirst();
	frame_counter++;
}

TsEnhancedFrameBufferBase::~TsEnhancedFrameBufferBase()
{
	delete sender;
}

TsEnhancedVideoFrameBuffer::TsEnhancedVideoFrameBuffer(es_source_types _source, int _index)
{
	initializeTs(_source, _index, ES_VIDEO, PTS_PER_FRAME_VIDEO, FRAME_PER_SECOND_VIDEO);
	if(is_primary_stream)
		initSender(video_decoder_protocol, video_decoder_ip_base, video_decoder_port_base);
	else
		initSender(video_decoder_protocol, video_secondary_decoder_ip_base, video_secondary_decoder_port_base);
}

TsEnhancedFrame *TsEnhancedVideoFrameBuffer::getFrameByPTS(unsigned int PTS)
{
	writeDebugMessage(buffer_id + "_request.txt", to_str(PTS) + "\t" + now_str(), INFO);
	/*if(isEmpty() || isFull())
		return NULL;*/
	
	string error_string = "buffer " + buffer_id + " cannot send " + to_str(PTS) + " because it is ";
	if(isEmpty())
	{
		printDebugMessage(error_string + "empty", INFO);
		return NULL;
	}
	else if(isFull())
	{
		printDebugMessage(error_string + "full", INFO);
		//return NULL;
	}
		
	int frame_difference = ((int)first->packet->PTS - (int)PTS) / (int)pts_per_frame;

	if(first->packet->PTS == PTS || frame_difference == 0)
	{
		writeDebugMessage(buffer_id + "_send.txt", to_str(first->packet->PTS) + "\t" + now_str() + " " + getStatusStr() + " " + to_str(getFillRatio()) + "%" + " " + to_str(current_size) + "/" + to_str(total_size), STATUS);
		first->packet->addPTSOffset(source == ES_SOURCE_DVB ? "DVB" : "P2P");
		return first->packet;
	}
	
	if(getFirstPTS() < PTS)
	{
		/*if(loop_on)
		{
			return NULL;
		}
		else
		{*/
			printDebugMessage("buffer " + buffer_id + " resync", INFO);
			dropUntil(PTS);
			return getFrameByPTS(PTS);
		//}
	}	
	
	printDebugMessage("buffer " + buffer_id + " missed " + to_str(frame_difference) + " frames in send", WARNING);
	return NULL;
}

bool TsEnhancedVideoFrameBuffer::sendFrameByPTS(unsigned int *PTS)
{
	TsEnhancedFrame *frame = getFrameByPTS(*PTS);
	if(frame != NULL)
	{
		writeDebugMessage(buffer_id + "_frame_time.txt", to_str(*PTS) + "\t" + to_str(frame->getArrivalTime()) + "\t" + to_str(frame->getDepartureTime()), STATUS);
		sendOK(frame);
		return true;
	}
	else
	{
		//checkLoop(PTS);
	}
	return false;
}

TsEnhancedVideoFrameBuffer::~TsEnhancedVideoFrameBuffer(){}

TsEnhancedAudioFrameBuffer::TsEnhancedAudioFrameBuffer(es_source_types _source, int _index)
{
	initializeTs(_source, _index, ES_AUDIO, PTS_PER_FRAME_AUDIO, FRAME_PER_SECOND_AUDIO);
	initSender(source == ES_SOURCE_DVB ? audio_decoder_protocol : audio_secondary_decoder_protocol, source == ES_SOURCE_DVB ? audio_decoder_ip_base : audio_secondary_decoder_ip_base, source == ES_SOURCE_DVB ? audio_decoder_port_base : audio_secondary_decoder_port_base - 1);
}

TsEnhancedFrame *TsEnhancedAudioFrameBuffer::getFrameByPTS(unsigned int PTS)
{
	if(first == NULL)
		return NULL;
	if((first->packet->PTS < PTS + PTS_PER_FRAME_VIDEO / 2) && (first->packet->PTS > PTS - PTS_PER_FRAME_VIDEO / 2))
	{
		writeDebugMessage(buffer_id + "_send.txt", to_str(first->packet->PTS) + "\t" + now_str() + " " + getStatusStr() + " " + to_str(getFillRatio()) + "%" + " " + to_str(current_size) + "/" + to_str(total_size), STATUS);
		first->packet->addPTSOffset(source == ES_SOURCE_DVB ? "DVB" : "P2P");
		return first->packet;
	}

	return NULL;
}

bool TsEnhancedAudioFrameBuffer::sendFrameByPTS(unsigned int *PTS)
{
	//cout << "request for first new frame" << endl;
	TsEnhancedFrame *frame = getFrameByPTS(*PTS);
	//cout << "receive the first new frame" << endl;
	while(frame != NULL)
	{
		//cout << "sending" << endl;
		sendOK(frame);
		//cout << "request for new frame" << endl;
		frame = getFrameByPTS(*PTS);
		//cout << "receive the new frame" << endl;
	}
	/*if(source == ES_SOURCE_P2P)
		checkLoop(PTS);*/
	//cout << "complete loop" << endl;
	return true;
}

TsEnhancedAudioFrameBuffer::~TsEnhancedAudioFrameBuffer(){}

TsEnhancedPrivateDataFrameBuffer::TsEnhancedPrivateDataFrameBuffer(es_source_types _source, int _index)
{
	initializeTs(_source, _index, ES_PRIVATE_DATA, PTS_PER_FRAME_PRIVATE_DATA, FRAME_PER_SECOND_PRIVATE_DATA);
	initSender(private_data_decoder_protocol, private_data_decoder_ip_base, private_data_decoder_port_base-1);
}

void TsEnhancedPrivateDataFrameBuffer::insertNewNode(TsEnhancedFrame *packet)
{
	//unsigned int first_PTS = first->packet->PTS;
	unsigned int received_PTS = packet->PTS;
	//unsigned int expected_PTS = last->packet->PTS + pts_per_frame;
	//int frame_difference = ((int)received_PTS - (int)expected_PTS) / (int)pts_per_frame;
	
	writeDebugMessage(buffer_id + "_insert.txt", to_str(received_PTS) + "\t" + now_str() + " " + getStatusStr() + " " + to_str(getFillRatio()) + "%" + " " + to_str(current_size) + "/" + to_str(total_size), STATUS);

	insertTail(packet);
}

TsEnhancedFrame *TsEnhancedPrivateDataFrameBuffer::getFrameByPTS(unsigned int PTS)
{
	if(first == NULL)
		return NULL;
	
	writeDebugMessage(buffer_id + "_send.txt", to_str(first->packet->PTS) + "\t" + now_str() + " " + getStatusStr() + " " + to_str(getFillRatio()) + "%" + " " + to_str(current_size) + "/" + to_str(total_size), STATUS);
	first->packet->addPTSOffset(source == ES_SOURCE_DVB ? "DVB" : "P2P");
	return first->packet;
	
	/*if((first->packet->PTS < PTS + PTS_PER_FRAME_VIDEO / 2) && (first->packet->PTS > PTS - PTS_PER_FRAME_VIDEO / 2))
	{
		writeDebugMessage(buffer_id + "_send.txt", to_str(first->packet->PTS) + "\t" + now_str() + " " + getStatusStr() + " " + to_str(getFillRatio()) + "%" + " " + to_str(current_size) + "/" + to_str(total_size), STATUS);
		first->packet->addPTSOffset(source == ES_SOURCE_DVB ? "DVB" : "P2P");
		return first->packet;
	}

	return NULL;*/
}

bool TsEnhancedPrivateDataFrameBuffer::sendFrameByPTS(unsigned int *PTS)
{
	TsEnhancedFrame *frame = getFrameByPTS(*PTS);
	if(frame != NULL)
	{
		int num_of_packets = frame->size / TS_PACKET_SIZE;
		for(int i=0; i<num_of_packets; i++)
		{
			sender->send(frame->data+i*TS_PACKET_SIZE, TS_PACKET_SIZE);
			//if(write_output_to_file_flag)
			//{
				writeRawData(to_str(decoder_port) + "/" + to_str(decoder_port), frame->data+i*TS_PACKET_SIZE, TS_PACKET_SIZE);
				//writeRawData(to_str(decoder_port) + "/" + to_str(decoder_port) + "_" + to_str(frame_counter), frame->data+i*TS_PACKET_SIZE, TS_PACKET_SIZE);
			//}
			frame_counter++;
			if(frame_counter == 1)
				printDebugMessage(buffer_id + " first PTS to send: " + to_str(frame->PTS) + "\t" + now_str(), INFO);
		}
		dropFirst();
		printDebugMessage(to_str(frame->PTS) + "\t" + now_str()+ "\t" + to_str(frame->size) + "\t" + decoder_ip + ":" + to_str(decoder_port) + "\t", LOOP);
		frame = getFrameByPTS(*PTS);
	}
	/*if(source == ES_SOURCE_P2P)
		checkLoop(PTS);*/
	return true;
}

TsEnhancedPrivateDataFrameBuffer::~TsEnhancedPrivateDataFrameBuffer(){}
