#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <string.h>

#include "TsProgram.h"
#include "debugMessagePrinter.h"

/* manually created PAT and PMT data just for VLC players */
unsigned char PATpacket[188] = {0x47, 0x40, 0x0, 0x10, 0x0, 0x0, 0xb0, 0x11, 0x7, 0xcf, 0xc1, 0x0, 0x0, 0x0, 0x0, 0xe0, 0x10, 0x0, 0x1, 0xe0, 0x20, 0x66, 0xa9, 0xa1, 0x15, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char PMTpacket[188] = {0x47, 0x40, 0x20, 0x10, 0x0, 0x2, 0xb0, 0x27, 0x0, 0x1, 0xc1, 0x0, 0x0, 0xe3, 0xe9, 0xf0, 0x0, 0x1b, 0xe3, 0xe9, 0xf0, 0x9, 0x28, 0x4, 0x4d, 0x40, 0x28, 0x1f, 0x52, 0x1, 0x0, 0xf, 0xe7, 0xd2, 0xf0, 0x7, 0x52, 0x1, 0x1, 0x7c, 0x2, 0x2e, 0x0, 0xb9, 0x63, 0x45, 0xca, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

using namespace std;

TsProgramBase::TsProgramBase()
{
	source = ES_SOURCE_DVB;
	index = 0;
	pmt_pid = 0x2000;
	pcr_pid = 0x2000;
	pmt_packet = NULL;
	si_tables_not_sent = true;
	pts2send = 0;
	pts2insert = 0;

	video_pid_base = 0x2000;
	video_pid_enhancement = 0x2000;
	video_frame_counter = 0;
	video_stream_base = NULL;
	video_stream_enhancement = NULL;
	enhanced_video_frame_buffer = NULL;
	lost_video_frame_counter = 0;

	audio_pid = 0x2000;
	audio_frame_counter = 0;
	audio_stream = NULL;
	enhanced_audio_frame_buffer = NULL;
	lost_audio_frame_counter = 0;

	private_data_pid = 0;
	private_data_frame_counter = 0;
	private_data_stream = NULL;
	enhanced_private_data_frame_buffer = NULL;
	lost_private_data_frame_counter = 0;

	program_id = "NO_ID";
}

void TsProgramBase::initialize(es_source_types _source, int _index, es_types _type)
{
	source = _source;
	index = _index;
	type = _type;
	program_id = "pr" + to_str(source) + to_str(index) + to_str(type);
}

void TsProgramBase::initVideoStreamBase()
{
	video_stream_base = new TsVideoEs(source, index, ES_BASE);
	enhanced_video_frame_buffer = new TsEnhancedVideoFrameBuffer(source, index);
}

void TsProgramBase::initVideoStreamEnhancement()
{
	video_stream_enhancement = new TsVideoEs(source, index, ES_ENHANCEMENT);
}

void TsProgramBase::initAudioStream()
{
	audio_stream = new TsAudioEs(source, index, ES_BASE);
	enhanced_audio_frame_buffer = new TsEnhancedAudioFrameBuffer(source, index);
}

void TsProgramBase::initPrivateDataStream()
{
	private_data_stream = new TsPrivateDataEs(source, index, ES_BASE);
	enhanced_private_data_frame_buffer = new TsEnhancedPrivateDataFrameBuffer(source, index);
}

void TsProgramBase::prepareAudioFrame()
{
	TsFrameBase *audio_frame = audio_stream->buffer->getFrameByPTS(pts2insert);
	while(audio_frame != NULL)
	{
		if(audio_frame->data != NULL)
			enhanced_audio_frame_buffer->insert(new TsEnhancedFrame(audio_frame));
		else
			printDebugMessage(program_id + " NULL audio frame with PTS: " + to_str(pts2insert), ERROR);
		audio_stream->buffer->dropFirst();
		audio_frame = audio_stream->buffer->getFrameByPTS(pts2insert);
	}
}

void TsProgramBase::preparePrivateDataFrame()
{
	TsFrameBase *private_data_frame = private_data_stream->buffer->getFrameByPTS(pts2insert);
	if(private_data_frame != NULL)
	{
		if(private_data_frame->data != NULL)
			enhanced_private_data_frame_buffer->insert(new TsEnhancedFrame(private_data_frame, false));
		else
			printDebugMessage(program_id + " NULL private data frame with PTS: " + to_str(pts2insert), ERROR);
		private_data_stream->buffer->dropFirst();
		private_data_frame = private_data_stream->buffer->getFrameByPTS(pts2insert);
	}
}

void TsProgramBase::resyncAudioStream()
{
	int counter = 0;
	while(audio_stream->buffer->getFrameByPTS(pts2insert) == NULL)
	{
		if(audio_stream->buffer->getStatus() == buffer_empty)
		{
			printDebugMessage(program_id + " Audio Buffer could not be resynced after LOOP", WARNING);
			break;
		}
			
		audio_stream->buffer->dropFirst();
		counter++;
	}
	if(counter > 0)
		printDebugMessage(program_id + " to resync Audio Buffer after LOOP " + to_str(counter) + " frames have been dropped", WARNING);
}

void TsProgramBase::setPmtPacket(TsPacket *packet)
{
	if(pmt_packet == NULL)
	{
		pmt_packet = new TsPacket();
		*pmt_packet = *packet;

		setVideoPidBase(pmt_packet->packet_payload->video_stream_list[0].elementary_pid);
		setAudioPid(pmt_packet->packet_payload->audio_stream_list[0].elementary_pid);
		setPcrPid(pmt_packet->packet_payload->pcr_pid);
		
		if(output_for_vlc_flag)
		{
			enhanced_video_frame_buffer->sender->send(pmt_packet->bytes, TS_PACKET_SIZE);
			enhanced_audio_frame_buffer->sender->send(pmt_packet->bytes, TS_PACKET_SIZE);
		}
	}
}

void TsProgramBase::setPmtPid(unsigned short int pid)
{
	pmt_pid = pid;
	printDebugMessage(program_id + " pmt pid: " + to_str(pmt_pid), INFO);
}

void TsProgramBase::setVideoPidBase(unsigned short int pid)
{
	if(pid == 1000) pid = 1001;
	video_pid_base = pid;
	video_stream_base->pid = video_pid_base;
	printDebugMessage(program_id + " video base pid: " + to_str(video_pid_base), INFO);
}

void TsProgramBase::setVideoPidEnhancement(unsigned short int pid)
{
	if(pid == 1001) pid = 1000;
	video_pid_enhancement = pid;
	video_stream_enhancement->pid = video_pid_enhancement;
	printDebugMessage(program_id + " video enhancement pid: " + to_str(video_pid_enhancement), INFO);
}

void TsProgramBase::setAudioPid(unsigned short int pid)
{
	audio_pid = pid;
	audio_stream->pid = audio_pid;
	printDebugMessage(program_id + " audio pid: " + to_str(audio_pid), INFO);
}

void TsProgramBase::setPrivateDataPid(unsigned short int pid)
{
	private_data_pid = pid;
	private_data_stream->pid = private_data_pid;
	printDebugMessage(program_id + " private data pid: " + to_str(private_data_pid), INFO);
}

void TsProgramBase::setPcrPid(unsigned short int pid)
{
	pcr_pid = pid;
	printDebugMessage(program_id + " pcr pid: " + to_str(pcr_pid), INFO);
}

bool TsProgramBase::isPmtPacket(unsigned short int pid)
{
	return pmt_pid == pid;
}

bool TsProgramBase::isPesVideoPacket(unsigned short int pid)
{
	return no_video_flag ? false : (video_pid_base == pid) || (video_pid_enhancement == pid);
}

bool TsProgramBase::isPesAudioPacket(unsigned short int pid)
{
	return no_audio_flag ? false : audio_pid == pid;
}

bool TsProgramBase::isPesPrivateDataPacket(unsigned short int pid)
{
	return no_private_data_flag ? false : private_data_pid == pid;
}

bool TsProgramBase::isPcrPacket(unsigned short int pid)
{
	return ((pcr_pid == pid) ? true : (pid == 1001));
}

TsPacket* TsProgramBase::getPmtPacket()
{
	return pmt_packet;
}

void TsProgramBase::addPesPacket(TsPacket *packet)
{
	if(packet->packet_payload != NULL && !ts_parser_mode)
		usePesPacket(packet);
}

void TsProgramBase::addPesVideoBasePacket(TsPacket *packet)
{
	video_stream_base->addPesPacket(packet);
}

void TsProgramBase::addPesVideoEnhancementPacket(TsPacket *packet)
{
	video_stream_enhancement->addPesPacket(packet);
}

void TsProgramBase::addPesAudioPacket(TsPacket *packet)
{
	audio_stream->addPesPacket(packet);
}

void TsProgramBase::addPesPrivateDataPacket(TsPacket *packet)
{
	private_data_stream->addPesPacket(packet);
}

TsProgramBase::~TsProgramBase()
{
	delete video_stream_base;
	delete video_stream_enhancement;
	delete enhanced_video_frame_buffer;
	delete audio_stream;
	delete enhanced_audio_frame_buffer;
	delete private_data_stream;
	delete enhanced_private_data_frame_buffer;
}

TsProgramDVB::TsProgramDVB()
{
	initialize(ES_SOURCE_DVB, 0, ES_ALL);
	initVideoStreamBase();
	initAudioStream();
}

void TsProgramDVB::usePesPacket(TsPacket *packet)
{
	if(packet->packet_type == TS_PES_VIDEO_PACKET)
		addPesVideoBasePacket(packet);
	else if(packet->packet_type == TS_PES_AUDIO_PACKET)
		addPesAudioPacket(packet);
	else if(packet->packet_type == TS_PES_PRIVATE_DATA_PACKET)
		addPesPrivateDataPacket(packet);
}

unsigned int TsProgramDVB::getFirstPTS()
{
	//if(no_video_flag)
	//	return max(video_stream_base->getFirstPTS(), audio_stream->getFirstPTS());
	unsigned int first_video_pts = video_stream_base->getFirstPTS();
	unsigned int first_audio_pts = audio_stream->getFirstPTS();
	cout << "First video PTS: " << first_video_pts << endl;
	cout << "First audio PTS: " << first_audio_pts << endl;
	return video_stream_base->getFirstPTS();
}

void TsProgramDVB::syncByPTS(unsigned int PTS)
{
	pts2insert = PTS;
	pts2send = PTS;
	video_stream_base->syncByPTS(PTS);
	audio_stream->syncByPTS(PTS);
}

void TsProgramDVB::prepareFrame()
{
	int increase_pts = false;
	TsFrameBase *video_frame = video_stream_base->buffer->getFrameByPTS(pts2insert);
	if(video_frame != NULL)
	{
		if(video_frame->data != NULL)
		{
			enhanced_video_frame_buffer->insert(new TsEnhancedFrame(video_frame));
		}
		else
		{
			printDebugMessage(program_id + " NULL video frame with PTS: " + to_str(pts2insert), ERROR);
		}
		video_stream_base->buffer->dropFirst();
		increase_pts = true;
	}
	else
	{
		/*if(video_stream_base->buffer->loop_on)
		{
			printDebugMessage(program_id + " loop at " + now_str(), WARNING);
			video_stream_base->buffer->loop_on = false;
			pts2insert = video_stream_base->buffer->getFirstPTS();
			resyncAudioStream();
		}
		else
		{*/
			printDebugMessage(program_id + " NO DVB video frame with PTS: " + to_str(pts2insert) + " <-> " + to_str(video_stream_base->buffer->getFirstPTS()) + " " + now_str(), ERROR);
			lost_video_frame_counter++;
			//writeDebugMessage(program_id + "_frame_lost.txt", to_str(lost_video_frame_counter) + "\t" + to_str(pts2insert) + "\t" + now_str(), INFO);
			increase_pts = true;
		//}
	}
	prepareAudioFrame();
	pts2insert = increase_pts ? pts2insert + PTS_PER_FRAME_VIDEO : pts2insert;
}

void TsProgramDVB::sendFrame()
{
	enhanced_video_frame_buffer->sendFrameByPTS(&pts2send);
	enhanced_audio_frame_buffer->sendFrameByPTS(&pts2send);
	pts2send += PTS_PER_FRAME_VIDEO;
}

void TsProgramDVB::printBufferStatus()
{
	video_stream_base->printBufferStatus();
	audio_stream->printBufferStatus();
}

TsProgramDVB::~TsProgramDVB(){}

TsProgramP2PVideo::TsProgramP2PVideo(int _index, unsigned short int pid)
{
	initialize(ES_SOURCE_P2P, _index, ES_VIDEO);
	initVideoStreamBase();
	initVideoStreamEnhancement();
	setVideoPidBase(pid);
	setVideoPidEnhancement(pid+1);
}

void TsProgramP2PVideo::usePesPacket(TsPacket *packet)
{
	if(packet->header->pid == video_pid_base)
		addPesVideoBasePacket(packet);
	else
		addPesVideoEnhancementPacket(packet);
}

unsigned int TsProgramP2PVideo::getFirstPTS()
{
	return max(video_stream_base->getFirstPTS(), video_stream_enhancement->getFirstPTS());
}

void TsProgramP2PVideo::syncByPTS(unsigned int PTS)
{
	pts2insert = PTS;
	pts2send = PTS;
	video_stream_base->syncByPTS(PTS);
	video_stream_enhancement->syncByPTS(PTS);
}

void TsProgramP2PVideo::prepareFrame()
{
	TsFrameBase *video_frame_base = video_stream_base->buffer->getFrameByPTS(pts2insert);
	if(video_frame_base != NULL)
	{
		TsFrameBase *video_frame_enhancement = video_stream_enhancement->buffer->getFrameByPTS(pts2insert);
		if(video_frame_enhancement != NULL)
		{
			if(video_frame_base->data != NULL && video_frame_enhancement->data != NULL)
				enhanced_video_frame_buffer->insert(new TsEnhancedFrame(video_frame_base, video_frame_enhancement));
			video_stream_enhancement->buffer->dropFirst();
		}
		else
		{
			if(video_frame_base->data != NULL)
				enhanced_video_frame_buffer->insert(new TsEnhancedFrame(video_frame_base));
		}
		video_stream_base->buffer->dropFirst();
		pts2insert += PTS_PER_FRAME_VIDEO;
	}
	else
	{
		printDebugMessage(program_id + " NO P2P video frame with PTS: " + to_str(pts2insert) + " <-> " + to_str(video_stream_base->buffer->getFirstPTS()) + " " + now_str(), ERROR);
		lost_video_frame_counter++;
		if(video_stream_base->buffer->resyncByPTS(pts2insert))
		{
			video_stream_enhancement->buffer->resyncByPTS(pts2insert);
			prepareFrame();
			return;
		}
		video_stream_enhancement->buffer->getFrameByPTS(pts2insert);
		pts2insert += PTS_PER_FRAME_VIDEO;
		//writeDebugMessage(program_id + "_frame_lost.txt", to_str(lost_video_frame_counter) + "\t" + to_str(pts2insert) + "\t" + now_str(), INFO);
	}
}

void TsProgramP2PVideo::sendFrame()
{
	/*bool is_send_ok = enhanced_video_frame_buffer->sendFrameByPTS(&pts2send);
	unsigned int pts_cand = enhanced_video_frame_buffer->getFirstPTS();
	pts2send = is_send_ok ? pts2send + PTS_PER_FRAME_VIDEO : (pts_cand > 0 ? pts_cand : pts2send);*/
	enhanced_video_frame_buffer->sendFrameByPTS(&pts2send);
	pts2send += PTS_PER_FRAME_VIDEO;
}

void TsProgramP2PVideo::printBufferStatus()
{
	video_stream_base->printBufferStatus();
	video_stream_enhancement->printBufferStatus();
}

TsProgramP2PVideo::~TsProgramP2PVideo(){}

TsProgramP2PAudio::TsProgramP2PAudio(int _index, unsigned short int pid)
{
	initialize(ES_SOURCE_P2P, _index, ES_AUDIO);
	initAudioStream();
	setAudioPid(pid);
}

void TsProgramP2PAudio::usePesPacket(TsPacket *packet)
{
	addPesAudioPacket(packet);
}

unsigned int TsProgramP2PAudio::getFirstPTS()
{
	//if(no_video_flag)
		return audio_stream->getFirstPTS();
	//return 1;
}

void TsProgramP2PAudio::syncByPTS(unsigned int PTS)
{
	pts2insert = PTS;
	pts2send = PTS;
	audio_stream->syncByPTS(PTS);
}

void TsProgramP2PAudio::prepareFrame()
{
	prepareAudioFrame();
	pts2insert += PTS_PER_FRAME_VIDEO;
	/*if(audio_stream->buffer->loop_on)
	{
		printDebugMessage(program_id + " loop at " + now_str(), WARNING);
		audio_stream->buffer->loop_on = false;
		pts2insert = audio_stream->buffer->getFirstPTS();
	}*/
}

void TsProgramP2PAudio::sendFrame()
{
	enhanced_audio_frame_buffer->sendFrameByPTS(&pts2send);
	pts2send += PTS_PER_FRAME_VIDEO;
}

void TsProgramP2PAudio::printBufferStatus()
{
	audio_stream->printBufferStatus();
}

TsProgramP2PAudio::~TsProgramP2PAudio(){}

TsProgramP2PPrivateData::TsProgramP2PPrivateData(int _index, unsigned short int pid)
{
	initialize(ES_SOURCE_P2P, _index, ES_PRIVATE_DATA);
	initPrivateDataStream();
	setPrivateDataPid(pid);
}

void TsProgramP2PPrivateData::usePesPacket(TsPacket *packet)
{
	addPesPrivateDataPacket(packet);
}

unsigned int TsProgramP2PPrivateData::getFirstPTS()
{
	//return 1;
	return private_data_stream->getFirstPTS();
}

void TsProgramP2PPrivateData::syncByPTS(unsigned int PTS)
{
	pts2insert = PTS;
	pts2send = PTS;
	private_data_stream->syncByPTS(PTS);
}

void TsProgramP2PPrivateData::prepareFrame()
{
	preparePrivateDataFrame();
	pts2insert += PTS_PER_FRAME_VIDEO;
	/*if(private_data_stream->buffer->loop_on)
	{
		printDebugMessage(program_id + " loop at " + now_str(), WARNING);
		private_data_stream->buffer->loop_on = false;
		pts2insert = private_data_stream->buffer->getFirstPTS();
	}*/
}

void TsProgramP2PPrivateData::sendFrame()
{
	enhanced_private_data_frame_buffer->sendFrameByPTS(&pts2send);
	pts2send += PTS_PER_FRAME_VIDEO;
}

void TsProgramP2PPrivateData::printBufferStatus()
{
	private_data_stream->printBufferStatus();
}

TsProgramP2PPrivateData::~TsProgramP2PPrivateData(){}
