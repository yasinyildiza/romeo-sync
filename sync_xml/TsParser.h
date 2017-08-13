#ifndef _TS_PARSER_H
#define _TS_PARSER_H

#include <pthread.h>

#include "TsProgram.h"
#include "Thread.h"
#include "SocketClient.h"

#ifndef MOBILE_PLATFORM
	#include "CurlOutput.h"
#endif

class TsParserBase : public Thread
{
	protected:
		
		pthread_mutex_t mutex;
		TsPacket *packet;

		unsigned short int *pid_list;
		unsigned int pid_list_length;
		unsigned char *last_cc_list;
		unsigned short int *cc_error_number_list;
		
		void initialize(es_source_types _source, int _index, es_types _type, int _pid_list_length);
		void lock();
		void release();
		int getPidIndex();
		void add2PidList();
		void checkCC();
		virtual void setPacketType() = 0;
		virtual void useParsedPacket() = 0;
		virtual void run();

	public:

		es_source_types_t source;
		int index;
		es_types type;

		TsProgramBase *program;
		
		TsParserBase();
		unsigned int getFirstPTS();
		void syncByPTS(unsigned int PTS);
		void prepareFrame();
		void sendFrame();
		void printBufferStatus();
		void parse(unsigned char bytes[188]);
		bool isPcrPacket();
		virtual ~TsParserBase();
};

class TsParserDVB : public TsParserBase
{
	private:

#ifndef MOBILE_PLATFORM
		CurlOutput *xml_output;
		TiXmlDocument *xml_doc;
#endif

		TsPacket *pat_packet;
		TsPacket *sdt_packet;
		TsPacket *eit_packet;
		TsPacket *bat_packet;
		TsPacket *nit_packet;
		unsigned short int nit_pid;

		SocketClient *video_pcr_sender;
		SocketClient *audio_pcr_sender;
		SocketClient *audio_scd_pcr_sender;
		SocketClient *private_data_pcr_sender;
		
		void setPacketType();
		void createPatPacket();
		int getUserProgramNumberChoice(unsigned int number_of_programs);
		bool isPmtPid();
		bool isPmtPacket();
		bool isPesVideoPacket();
		bool isPesAudioPacket();
		bool isPesPrivateDataPacket();
		void processAdditionalContentUrl();
		void run();
	public:
		TsParserDVB();
		void useParsedPacket();
		~TsParserDVB();
};

class TsParserP2PBase : public TsParserBase
{
	private:
		unsigned short int es_pid;
		
		virtual void setPacketType();
	
	public:
		TsParserP2PBase();
		void initTs(int _index);
		virtual void useParsedPacket();
		virtual ~TsParserP2PBase();
};

class TsParserP2PVideoBase : public TsParserP2PBase
{
	private:
		SocketClient *video_pcr_sender;
		SocketClient *audio_pcr_sender;
		SocketClient *audio_scd_pcr_sender;
		SocketClient *private_data_pcr_sender;
		
		
	public:
		TsParserP2PVideoBase();
		TsParserP2PVideoBase(int _index, unsigned short int _es_pid);
		void useParsedPacket();
		virtual ~TsParserP2PVideoBase();
};

class TsParserP2PVideoEnhancement : public TsParserP2PBase
{
	public:
		TsParserP2PVideoEnhancement(TsParserBase *parser);
		~TsParserP2PVideoEnhancement();
};

class TsParserP2PAudio : public TsParserBase
{
	private:
		void setPacketType();
	public:
		TsParserP2PAudio(int _index, unsigned short int _es_pid);
		void useParsedPacket();
		~TsParserP2PAudio();
};

class TsParserP2PPrivateData : public TsParserBase
{
	private:
		unsigned short int es_pid;
		void setPacketType();
	public:
		TsParserP2PPrivateData(int _index, unsigned short int _es_pid);
		void useParsedPacket();
		~TsParserP2PPrivateData();
};

#endif
