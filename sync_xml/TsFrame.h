#ifndef _TS_FRAME_H
#define _TS_FRAME_H

#include "TsPes.h"
#include "FrameBase.h"

class TsFrameBase : public FrameBase
{
	protected:

		virtual unsigned int getOffset(unsigned int offset);
		void addData(unsigned int offset, TsPacket *packet);

	public:

		TsPes *current_ts_pes;
		unsigned int PTS;
		unsigned int size;
		unsigned char *data;
		bool has_error;
		es_types type;
		es_subtypes subtype;

		TsFrameBase();
		void initializeTs(TsPacket *packet, es_types _type, es_subtypes _subtype);
		TsFrameBase(const TsFrameBase &other);
		TsFrameBase &operator=(const TsFrameBase &rhs);
		void addPesPacket(TsPacket *packet);
		virtual ~TsFrameBase();
};

class TsVideoFrame : public TsFrameBase
{
	public:
		TsVideoFrame();
		TsVideoFrame(TsPacket *packet, es_subtypes _subtype);
		~TsVideoFrame();
};

class TsAudioFrame : public TsFrameBase
{
	public:
		TsAudioFrame();
		TsAudioFrame(TsPacket *packet, es_subtypes _subtype);
		~TsAudioFrame();
};

class TsPrivateDataFrame : public TsFrameBase
{
	public:
		TsPrivateDataFrame();
		unsigned int getOffset(unsigned int offset);
		TsPrivateDataFrame(TsPacket *packet, es_subtypes _subtype);
		~TsPrivateDataFrame();
};

#endif
