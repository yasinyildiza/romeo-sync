#ifndef _TS_PROGRAM_H
#define _TS_PROGRAM_H

#include "TsEs.h"
#include "TsEnhancedFrameBuffer.h"

class TsProgramBase
{
	protected:

		std::string program_id;
	
		es_source_types source;
		int index;
		es_types type;
		unsigned short int pmt_pid;
		unsigned short int pcr_pid;
		TsPacket *pmt_packet;
		bool si_tables_not_sent;
		unsigned int pts2send;
		unsigned int pts2insert;

		unsigned short int video_pid_base;
		unsigned short int video_pid_enhancement;
		int video_frame_counter;
		TsVideoEs *video_stream_base;
		TsVideoEs *video_stream_enhancement;
		//TsEnhancedVideoFrameBuffer *enhanced_video_frame_buffer;
		int lost_video_frame_counter;

		unsigned short int audio_pid;
		int audio_frame_counter;
		TsAudioEs *audio_stream;
		//TsEnhancedAudioFrameBuffer *enhanced_audio_frame_buffer;
		int lost_audio_frame_counter;

		unsigned short int private_data_pid;
		int private_data_frame_counter;
		TsPrivateDataEs *private_data_stream;
		TsEnhancedPrivateDataFrameBuffer *enhanced_private_data_frame_buffer;
		int lost_private_data_frame_counter;

		void initialize(es_source_types _source, int _index, es_types _type);

		void initVideoStreamBase();
		void initVideoStreamEnhancement();
		void initAudioStream();
		void initPrivateDataStream();

		void addPesVideoBasePacket(TsPacket *packet);
		void addPesVideoEnhancementPacket(TsPacket *packet);
		void addPesAudioPacket(TsPacket *packet);
		void addPesPrivateDataPacket(TsPacket *packet);

		void prepareAudioFrame();
		void resyncAudioStream();

		void preparePrivateDataFrame();
		
	public:
		TsEnhancedVideoFrameBuffer *enhanced_video_frame_buffer;
		TsEnhancedAudioFrameBuffer *enhanced_audio_frame_buffer;

		TsProgramBase();

		void setPmtPid(unsigned short int pid);
		void setPmtPacket(TsPacket *packet);
		void setVideoPidBase(unsigned short int pid);
		void setVideoPidEnhancement(unsigned short int pid);
		void setAudioPid(unsigned short int pid);
		void setPrivateDataPid(unsigned short int pid);
		void setPcrPid(unsigned short int pid);
		
		bool isPmtPacket(unsigned short int pid);
		bool isPesVideoPacket(unsigned short int pid);
		bool isPesAudioPacket(unsigned short int pid);
		bool isPesPrivateDataPacket(unsigned short int pid);
		bool isPcrPacket(unsigned short int pid);
		
		TsPacket* getPmtPacket();

		void addPesPacket(TsPacket *packet);
		virtual void usePesPacket(TsPacket *packet) = 0;
		
		virtual unsigned int getFirstPTS() = 0;
		virtual void syncByPTS(unsigned int PTS) = 0;
		virtual void prepareFrame() = 0;
		virtual void sendFrame() = 0;
		virtual void printBufferStatus() = 0;
		virtual ~TsProgramBase();
};

class TsProgramDVB : public TsProgramBase
{
	private:
	public:
		TsProgramDVB();
		void usePesPacket(TsPacket *packet);
		unsigned int getFirstPTS();
		void syncByPTS(unsigned int PTS);
		void prepareFrame();
		void sendFrame();
		void printBufferStatus();
		~TsProgramDVB();
};

class TsProgramP2PVideo : public TsProgramBase
{
	private:
	public:
		TsProgramP2PVideo(int _index, unsigned short int pid);
		void usePesPacket(TsPacket *packet);
		unsigned int getFirstPTS();
		void syncByPTS(unsigned int PTS);
		void prepareFrame();
		void sendFrame();
		void printBufferStatus();
		~TsProgramP2PVideo();
};

class TsProgramP2PAudio : public TsProgramBase
{
	private:
	public:
		TsProgramP2PAudio(int _index, unsigned short int pid);
		void usePesPacket(TsPacket *packet);
		unsigned int getFirstPTS();
		void syncByPTS(unsigned int PTS);
		void prepareFrame();
		void sendFrame();
		void printBufferStatus();
		~TsProgramP2PAudio();
};

class TsProgramP2PPrivateData : public TsProgramBase
{
	private:
	public:
		TsProgramP2PPrivateData(int _index, unsigned short int pid);
		void usePesPacket(TsPacket *packet);
		unsigned int getFirstPTS();
		void syncByPTS(unsigned int PTS);
		void prepareFrame();
		void sendFrame();
		void printBufferStatus();
		~TsProgramP2PPrivateData();
};

#endif
