#ifndef _TS_TRANSRECEIVER_H
#define _TS_TRANSRECEIVER_H

#include <stdio.h>
#include <string.h>

#include "Thread.h"
#include "TsParser.h"
#include "TsPacketBuffer.h"
#include "ConfigFileParser.h"
#include "DvbSubdevices.h"
#include "UDPserver.h"

#define TS_PACKET_BUFFER_SIZE_AS_PACKETS 20000
#define TS_PACKET_BUFFER_SIZE_AS_BYTES (TS_PACKET_BUFFER_SIZE_AS_PACKETS * TS_PACKET_SIZE)

class TsReceiverBase
{
	private:
		pthread_mutex_t mutex;
		void lock();
		void release();
		
		int ts_packet_buffer_current_size;
		int write_offset;
		int read_offset;
		bool is_sending;

	protected:

		ConfigFileParser *conf_file;
		
		Thread *receive_ts_packets_thread;
		Thread *prepare_ts_frames_thread;
		Thread *prepare_ts_enhanced_frames_thread;
		Thread *send_ts_enhanced_frames_thread;
		Thread *read_measurement_values_thread;

		unsigned char *ts_packet_buffer;
		unsigned char *packet;

	public:
		
		es_source_types source;
		int index;
		es_types type;
		es_subtypes subtype;

		bool sending_frame_flag;
		std::string receiver_id;
		TsParserBase *ts_parser;
		pthread_mutex_t count_lock;
		pthread_cond_t count_nonzero;
		int count;
		pthread_mutex_t count_lock2;
		pthread_cond_t count_nonzero2;
		int count2;
		
		pthread_mutex_t buffer_lock;
		pthread_cond_t buffer_not_empty;
		int buffer_count;
		
		TsReceiverBase();
		void initialize(es_source_types _source, int _index, es_types _type, es_subtypes _subtype);

		void start2receiveTsPackets();
		void start2prepareTsFrames();
		virtual void start2prepareTsEnhancedFrames();
		virtual void start2sendTsEnhancedFrames();

		void writePacket();
		void readPacket();
		
		unsigned int getFirstPTS();
		virtual void syncByPTS(unsigned int PTS);
		void prepareFrame();
		virtual void sendFrame();
		virtual void printBufferStatus();
		
		virtual bool tune(TsParserBase *parser) = 0;
		virtual unsigned char *receivePacket() = 0;
		void tuned();
		void waitForAllowSend();
		void waitForSendComplete();
		void allowSend();
		void sendComplete();
		void stopThreads();
		virtual void stop() = 0;
		virtual void readMeasurementValues();
		virtual void printMeasurementValues();
		virtual ~TsReceiverBase();
};

class ReceiveTsPacketsThread : public Thread
{
	private:
		TsReceiverBase *receiver;
	public:
		ReceiveTsPacketsThread(TsReceiverBase *_receiver);
		void run();
		~ReceiveTsPacketsThread();
};

class PrepareTsFramesThread : public Thread
{
	private:
		TsReceiverBase *receiver;
	public:
		PrepareTsFramesThread(TsReceiverBase *_receiver);
		void run();
		~PrepareTsFramesThread();
};

class PrepareTsEnhancedFramesThread : public Thread
{
	private:
		TsReceiverBase *receiver;
	public:
		PrepareTsEnhancedFramesThread(TsReceiverBase *_receiver);
		void run();
		~PrepareTsEnhancedFramesThread();
};

class SendTsEnhancedFramesThread : public Thread
{
	private:
		TsReceiverBase *receiver;
	public:
		SendTsEnhancedFramesThread(TsReceiverBase *_receiver);
		void run();
		~SendTsEnhancedFramesThread();
};

class ReadMeasurementValuesThread : public Thread
{
	private:
		TsReceiverBase *receiver;
	public:
		ReadMeasurementValuesThread(TsReceiverBase *_receiver);
		void run();
		~ReadMeasurementValuesThread();
};

class TsReceiverDVB : public TsReceiverBase
{
	private:
	
		FeDevice* fe;
		DmxDevice* dmx;
		DvrDevice* dvr;
		
		int adapter_num;
		int fe_num;
		int dmx_num;
		int dvr_num;
		int num_of_packets_per_read;
		
		int tune_timeout;
		int frequency;
		fe_bandwidth_t bandwidth;
		fe_code_rate_t code_rate_HP;
		fe_code_rate_t code_rate_LP;
		fe_spectral_inversion_t inversion;
		fe_modulation_t constellation;
		fe_transmit_mode_t transmission_mode;
		fe_guard_interval_t guard_interval;
		fe_hierarchy_t hierarchy_information;
		
		void configure();

	public:
	
		TsReceiverDVB();
		TsReceiverDVB(	int _frequency,
						fe_bandwidth_t _bandwidth,
						fe_code_rate_t _code_rate_HP,
						fe_code_rate_t _code_rate_LP,
						fe_spectral_inversion_t _inversion,
						fe_modulation_t _constellation,
						fe_transmit_mode_t _transmission_mode,
						fe_guard_interval_t _guard_interval,
						fe_hierarchy_t _hierarchy_information);
		virtual bool tune(TsParserBase *parser);
		virtual unsigned char *receivePacket();
		virtual void stop();
		virtual void readMeasurementValues();
		virtual void printMeasurementValues();
		virtual ~TsReceiverDVB();
};

class TsReceiverP2PBase : public TsReceiverBase
{
	protected:

		int num_of_packets_per_read;
		unsigned char packet[188];
		unsigned char *buffer;
		int num_of_packets_read;
		int num_of_packets_serviced;
		int read_size;

		UDPserver *packet_receiver;
		int receiver_port_number;

		unsigned short int es_pid;

		void configure();
		void initializeTs(unsigned short int pid, int port_number);
		
	public:

		TsReceiverP2PBase();
		virtual bool tune(TsParserBase *parser) = 0;
		unsigned char *receivePacket();
		void stop();
		virtual ~TsReceiverP2PBase();
};

class TsReceiverP2PVideoBase : public TsReceiverP2PBase
{
	public:
		TsReceiverP2PVideoBase(int _index);
		bool tune(TsParserBase *parser);
		~TsReceiverP2PVideoBase();
};

class TsReceiverP2PVideoEnhancement : public TsReceiverP2PBase
{
	public:
		TsReceiverP2PVideoEnhancement(int _index);
		bool tune(TsParserBase *parser);
		void start2prepareTsEnhancedFrames();
		//void start2sendTsEnhancedFrames();
		void syncByPTS(unsigned int PTS);
		void sendFrame();
		void printBufferStatus();
		~TsReceiverP2PVideoEnhancement();
};

class TsReceiverP2PAudio : public TsReceiverP2PBase
{
	public:
		TsReceiverP2PAudio(int _index);
		bool tune(TsParserBase *parser);
		~TsReceiverP2PAudio();
};

class TsReceiverP2PPrivateData : public TsReceiverP2PBase
{
	public:
		TsReceiverP2PPrivateData(int _index);
		bool tune(TsParserBase *parser);
		~TsReceiverP2PPrivateData();
};

class TsReceiverTsFile : public TsReceiverDVB
{
	private: 
		std::string file_path;
		std::ifstream file_stream;
		char packet[188];
	public:
		TsReceiverTsFile();
		TsReceiverTsFile(std::string _file_path);
		bool tune(TsParserBase *parser);
		unsigned char *receivePacket();
		void stop();
		void readMeasurementValues();
		void printMeasurementValues();
		~TsReceiverTsFile();
};

#endif
