#ifndef _TS_FRAME_BUFFER_H
#define _TS_FRAME_BUFFER_H

#include "TsFrame.h"
#include "BufferBase.h"

typedef enum buffer_sync_status
{
	buffer_not_synced,
	buffer_syncing_waiting_for_pts,
	buffer_syncing_pts_received,
	buffer_synced,
	buffer_cannot_be_synced
} buffer_sync_status_t;

class TsFrameBufferBase : public BufferBase<TsFrameBase>
{
	protected:
		es_source_types source;
		int index;
		es_types type;
		es_subtypes subtype;
		unsigned int pts_per_frame;
		int frame_per_second;
		unsigned int sync_PTS;
		buffer_sync_status_t sync_status;
		
		void initializeTs(es_source_types _source, int _index, es_types _type, es_subtypes _subtype, int _pts_per_frame, int _frame_per_second);
		const bool PTSexists(unsigned int PTS);
		void dropUntil(unsigned int PTS);
		virtual void insertNullFrame(int number_of_missed_frames, unsigned int PTS) = 0;
	public:
		bool loop_on;
		TsFrameBufferBase();
		const unsigned int getFirstPTS();
		virtual bool resyncByPTS(unsigned int PTS);
		virtual void syncByPTS(unsigned int PTS);
		virtual void insertNewNode(TsFrameBase *packet);
		virtual TsFrameBase *getFrameByPTS(unsigned int PTS) = 0;
		virtual ~TsFrameBufferBase();
};

class TsVideoFrameBuffer : public TsFrameBufferBase
{
	private:
	public:
		TsVideoFrameBuffer(es_source_types _source, int _index, es_subtypes _subtype);
		void insertNullFrame(int number_of_missed_frames, unsigned int PTS);
		TsFrameBase *getFrameByPTS(unsigned int PTS);
		~TsVideoFrameBuffer();
};

class TsAudioFrameBuffer : public TsFrameBufferBase
{
	private:
	public:
		TsAudioFrameBuffer(es_source_types _source, int _index, es_subtypes _subtype);
		void insertNullFrame(int number_of_missed_frames, unsigned int PTS);
		TsFrameBase *getFrameByPTS(unsigned int PTS);
		~TsAudioFrameBuffer();
};

class TsPrivateDataFrameBuffer : public TsFrameBufferBase
{
	private:
	public:
		TsPrivateDataFrameBuffer(es_source_types _source, int _index, es_subtypes _subtype);
		void insertNullFrame(int number_of_missed_frames, unsigned int PTS);
		TsFrameBase *getFrameByPTS(unsigned int PTS);
		void syncByPTS(unsigned int PTS);
		void insertNewNode(TsFrameBase *packet);
		~TsPrivateDataFrameBuffer();
};

#endif
