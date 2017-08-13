#include <iostream>

#include "TsFrame.h"
#include "debugMessagePrinter.h"

using namespace std;

TsFrameBase::TsFrameBase()
{
	current_ts_pes = NULL;
	size = 0;
	PTS = 0;
	data = NULL;
	has_error = false;
	type = ES_VIDEO;
	subtype = ES_BASE;
}

void TsFrameBase::initializeTs(TsPacket *packet, es_types _type, es_subtypes _subtype)
{
	current_ts_pes = new TsPes(packet);
	size = 0;
	PTS = packet->packet_payload->header->PTS;
	data = new unsigned char[MAX_FRAME_SIZE];
	has_error = false;
	type = _type;
	subtype = _subtype;

	addData(packet->packet_payload->pes_payload_start_offset + PES_HEADER_FLAG_SIZE + packet->packet_payload->header->PES_header_data_len, packet);
}

TsFrameBase::TsFrameBase(const TsFrameBase &other)
{
	current_ts_pes = new TsPes();
	*current_ts_pes = *(other.current_ts_pes);
	size = other.size;
	PTS = other.PTS;
	type = other.type;
	has_error = other.has_error;
	data = new unsigned char[MAX_FRAME_SIZE];
	memcpy(data, other.data, size);
}

TsFrameBase &TsFrameBase::operator=(const TsFrameBase &rhs)
{
	delete current_ts_pes;
	delete []data;
	current_ts_pes = new TsPes();
	*current_ts_pes = *(rhs.current_ts_pes);
	size = rhs.size;
	PTS = rhs.PTS;
	type = rhs.type;
	has_error = rhs.has_error;
	data = new unsigned char[MAX_FRAME_SIZE];
	memcpy(data, rhs.data, size);
	return *this;
}

unsigned int TsFrameBase::getOffset(unsigned int offset)
{
	return offset;
}

void TsFrameBase::addData(unsigned int offset, TsPacket *packet)
{
	offset = output_for_vlc_flag ? 0 : getOffset(offset);
	memcpy(data+size, packet->bytes+offset, TS_PACKET_SIZE - offset);
	size += TS_PACKET_SIZE - offset;
}

void TsFrameBase::addPesPacket(TsPacket *packet)
{
	if(current_ts_pes == NULL)
	{
		current_ts_pes = new TsPes(packet);
		addData(packet->packet_payload->pes_payload_start_offset + PES_HEADER_FLAG_SIZE + packet->packet_payload->header->PES_header_data_len, packet);
		return;
	}
	if(packet->packet_payload->header->isPesStarted)
	{
		if(!(current_ts_pes->isEnded()))
		{
			has_error = true;
			printDebugMessage("pes must have already ended", ERROR);
		}
		delete current_ts_pes;
		current_ts_pes = NULL;
		addPesPacket(packet);
		return;
	}
	current_ts_pes->addPesPacket(packet);
	if(current_ts_pes->has_error)
		has_error = true;
	else
		addData(packet->packet_payload->pes_payload_start_offset, packet);
}

TsFrameBase::~TsFrameBase()
{
	delete current_ts_pes;
	delete []data;
}

TsVideoFrame::TsVideoFrame(){}

TsVideoFrame::TsVideoFrame(TsPacket *packet, es_subtypes _subtype)
{
	initializeTs(packet, ES_VIDEO, _subtype);
}

TsVideoFrame::~TsVideoFrame(){}

TsAudioFrame::TsAudioFrame(){}

TsAudioFrame::TsAudioFrame(TsPacket *packet, es_subtypes _subtype)
{
	initializeTs(packet, ES_AUDIO, _subtype);
}

TsAudioFrame::~TsAudioFrame(){}

TsPrivateDataFrame::TsPrivateDataFrame(){}

TsPrivateDataFrame::TsPrivateDataFrame(TsPacket *packet, es_subtypes _subtype)
{
	initializeTs(packet, ES_PRIVATE_DATA, _subtype);
}

unsigned int TsPrivateDataFrame::getOffset(unsigned int offset)
{
	//return TS_HEADER_SIZE;
	return 0;
}

TsPrivateDataFrame::~TsPrivateDataFrame(){}
