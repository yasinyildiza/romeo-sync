#ifndef _TS_ENHANCED_FRAME_BUFFER_H
#define _TS_ENHANCED_FRAME_BUFFER_H

#include "BufferBase.h"
#include "TsEnhancedFrame.h"
#include "SocketClient.h"
#include "globals.h"

class TsEnhancedFrameBufferBase : public BufferBase<TsEnhancedFrame>
{
	protected:
		es_source_types source;
		int index;
		es_types type;
		es_subtypes subtype;
		unsigned int pts_per_frame;
		int frame_per_second;
		std::string decoder_protocol;
		std::string decoder_ip;
		int decoder_port;
		int frame_counter;
		bool is_primary_stream;
	public:
		SocketClient *sender;
		bool loop_on;
		TsEnhancedFrameBufferBase();
		void initializeTs(es_source_types _source, int _index, es_types _type, int _pts_per_frame, int _frame_per_second);
		void initSender(std::string _decoder_protocol, std::string _decoder_ip, int _decoder_port_base);
		const unsigned int getFirstPTS();
		void dropUntil(unsigned int PTS);
		virtual void insertNewNode(TsEnhancedFrame *packet);
		virtual TsEnhancedFrame *getFrameByPTS(unsigned int PTS) = 0;
		virtual bool sendFrameByPTS(unsigned int *PTS) = 0;
		void sendOK(TsEnhancedFrame *frame);
		void checkLoop(unsigned int *PTS);
		virtual ~TsEnhancedFrameBufferBase();
};

class TsEnhancedVideoFrameBuffer : public TsEnhancedFrameBufferBase
{
	public:
		TsEnhancedVideoFrameBuffer(es_source_types _source, int _index);
		TsEnhancedFrame *getFrameByPTS(unsigned int PTS);
		bool sendFrameByPTS(unsigned int *PTS);
		~TsEnhancedVideoFrameBuffer();
};

class TsEnhancedAudioFrameBuffer : public TsEnhancedFrameBufferBase
{
	public:
		TsEnhancedAudioFrameBuffer(es_source_types _source, int _index);
		TsEnhancedFrame *getFrameByPTS(unsigned int PTS);
		bool sendFrameByPTS(unsigned int *PTS);
		~TsEnhancedAudioFrameBuffer();
};

class TsEnhancedPrivateDataFrameBuffer : public TsEnhancedFrameBufferBase
{
	public:
		TsEnhancedPrivateDataFrameBuffer(es_source_types _source, int _index);
		void insertNewNode(TsEnhancedFrame *packet);
		TsEnhancedFrame *getFrameByPTS(unsigned int PTS);
		bool sendFrameByPTS(unsigned int *PTS);
		~TsEnhancedPrivateDataFrameBuffer();
};

#endif
