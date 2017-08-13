#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "debugMessagePrinter.h"
#include "TsEnhancedFrame.h"

using namespace std;

TsEnhancedFrame::TsEnhancedFrame(TsFrameBase *base_frame)
{
	size = 8 + base_frame->size;
	setBase(base_frame);
}

TsEnhancedFrame::TsEnhancedFrame(TsFrameBase *base_frame, bool temp)
{
	size = base_frame->size;
	memcpy(size_str, &size, 4);
	PTS = base_frame->PTS;
	data = new unsigned char[size];
	memcpy(data, base_frame->data, base_frame->size);
	setArrivalTime(base_frame->getArrivalTime());
}

TsEnhancedFrame::TsEnhancedFrame(TsFrameBase *base_frame, TsFrameBase *enhancement_frame)
{
	size = 8 + base_frame->size + enhancement_frame->size;
	setBase(base_frame);
	memcpy(data+8+base_frame->size, enhancement_frame->data, enhancement_frame->size);
}

void TsEnhancedFrame::setBase(TsFrameBase *base_frame)
{
	memcpy(size_str, &size, 4);

	PTS = base_frame->PTS;
	memcpy(PTS_str, &PTS, 4);

	data = new unsigned char[size];
	memset(data, 0, 4);
	memcpy(data+4, PTS_str+3, 1);
	memcpy(data+5, PTS_str+2, 1);
	memcpy(data+6, PTS_str+1, 1);
	memcpy(data+7, PTS_str, 1);
	memcpy(data+8, base_frame->data, base_frame->size);
	
	setArrivalTime(base_frame->getArrivalTime());
}

void TsEnhancedFrame::addPTSOffset(string source_str)
{
	setDepartureTime();
	long long int time_in_buffer = getDepartureTime() - getArrivalTime();
	long long int PTS_offset = time_in_buffer * 0.090;
	//cout << source_str << "\t" << PTS << "\t" << size << "\t" << getArrivalTime() << "\t" << getDepartureTime() << "\t" << time_in_buffer << "\t" << PTS_offset << endl;
	//writeDebugMessage(source_str + "_frame_time", source_str + "\t" + to_str(PTS) + "\t" + to_str(size) + "\t" + to_str(getArrivalTime()) + "\t" + to_str(getDepartureTime()) + "\t" + to_str(time_in_buffer) + "\t" + to_str(PTS_offset), INFO);
	//cout << "time offset: " << getTimeOffsetAsSec() << endl;
	/*long long int PTS_offset = getTimeOffsetAsPTS();
	PTS += PTS_offset
	memcpy(PTS_str, &PTS, 4);
	memcpy(data+4, PTS_str+3, 1);
	memcpy(data+5, PTS_str+2, 1);
	memcpy(data+6, PTS_str+1, 1);
	memcpy(data+7, PTS_str, 1);*/
}

TsEnhancedFrame::~TsEnhancedFrame()
{
	delete []data;
}
