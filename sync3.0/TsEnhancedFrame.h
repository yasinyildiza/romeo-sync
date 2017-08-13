#ifndef _TS_ENHANCED_FRAME_H
#define _TS_ENHANCED_FRAME_H

#include "TsFrame.h"
#include "FrameBase.h"

class TsEnhancedFrame : public FrameBase
{
	private:
		void setBase(TsFrameBase *base_frame);
		unsigned char PTS_str[4];
	public:
		unsigned char *data;
		int size;
		unsigned int PTS;
		unsigned char size_str[4];

		TsEnhancedFrame(TsFrameBase *base_frame);
		TsEnhancedFrame(TsFrameBase *base_frame, bool temp);
		TsEnhancedFrame(TsFrameBase *base_frame, TsFrameBase *enhancement_frame);
		void addPTSOffset(std::string source_string);
		~TsEnhancedFrame();
};

#endif
