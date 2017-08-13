#include <iostream>

#include "TsEs.h"
#include "debugMessagePrinter.h"

using namespace std;

TsEsBase::TsEsBase()
{
	current_frame = NULL;
	source = ES_SOURCE_DVB;
	index = 0;
	type = ES_VIDEO;
	subtype = ES_BASE;
	buffer = NULL;
	pid = 8192;
}

void TsEsBase::initializeTs(es_source_types _source, int _index, es_types _type, es_subtypes _subtype)
{
	current_frame = NULL;
	source = _source;
	index = _index;
	type = _type;
	subtype = _subtype;
	buffer = NULL;
	pid = 8192;
}

void TsEsBase::addPesPacket(TsPacket *packet)
{
	if(current_frame == NULL)
	{
		if(packet->packet_payload->isStartOfFrame())
		{
			initializeFrame(packet);
		}
		return;
	}
	if(packet->packet_payload->isNewFrame(current_frame->PTS))
	{
		if(current_frame->has_error)
		{
			printDebugMessage("frame has error, dropped.", WARNING);
		}
		else
		{
			buffer->insert(current_frame);
		}
		//delete current_frame;
		current_frame = NULL;
		addPesPacket(packet);
		return;
	}
	current_frame->addPesPacket(packet);
}

unsigned int TsEsBase::getFirstPTS()
{
	return buffer->getFirstPTS();
}

void TsEsBase::syncByPTS(unsigned int PTS)
{
	buffer->syncByPTS(PTS);
}

void TsEsBase::printBufferStatus()
{
	buffer->printFillRatio();
}

TsEsBase::~TsEsBase()
{
	delete current_frame;
	delete buffer;
}

TsVideoEs::TsVideoEs(es_source_types _source, int _index, es_subtypes _subtype)
{
	initializeTs(_source, _index, ES_VIDEO, _subtype);
	buffer = new TsVideoFrameBuffer(source, index, subtype);
}

void TsVideoEs::initializeFrame(TsPacket *packet)
{
	current_frame = new TsVideoFrame(packet, subtype);
}

TsVideoEs::~TsVideoEs(){}

TsAudioEs::TsAudioEs(es_source_types _source, int _index, es_subtypes _subtype)
{
	initializeTs(_source, _index, ES_AUDIO, _subtype);
	buffer = new TsAudioFrameBuffer(source, index, subtype);
}

void TsAudioEs::initializeFrame(TsPacket *packet)
{
	current_frame = new TsAudioFrame(packet, subtype);
}

TsAudioEs::~TsAudioEs(){}

TsPrivateDataEs::TsPrivateDataEs(es_source_types _source, int _index, es_subtypes _subtype)
{
	initializeTs(_source, _index, ES_PRIVATE_DATA, _subtype);
	buffer = new TsPrivateDataFrameBuffer(source, index, subtype);
}

void TsPrivateDataEs::initializeFrame(TsPacket *packet)
{
	current_frame = new TsPrivateDataFrame(packet, subtype);
}

/*void TsPrivateDataEs::addPesPacket(TsPacket *packet)
{
	initializeFrame(packet);
	buffer->insert(current_frame);
	current_frame = NULL;
}*/

TsPrivateDataEs::~TsPrivateDataEs(){}
