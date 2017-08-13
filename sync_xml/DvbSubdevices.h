#ifndef _DVB_SUB_DEVICES_H
#define _DVB_SUB_DEVICES_H

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>
#include <string>

#include "frontend.h"
#include "dmx.h"
#include "DvbObjects.h"
#include "TsPacket.def"

typedef enum dvb_subdevice_type
{
	DVB_FRONTEND,
	DVB_DEMUX,
	DVB_DVR,
	UNKNOWN
} dvb_subdevice_type_t;

class DvbSubdevice
{
	private:
		
		void Xopen(int flags);
		std::string getSubDeviceName();
		
	protected:
		
		int adapter_num;
		int subdevice_num;
		dvb_subdevice_type_t subdevice_type;

		int fd;
		bool isDeviceOpen();
		
	public:
		
		DvbSubdevice();
		DvbSubdevice(int _adapter_num, int _subdevice_num, dvb_subdevice_type _subdevice_type);
		void openRD();
		void openRDWR();
		void Xclose();
		virtual void print() = 0;
		virtual ~DvbSubdevice();
};

class FeDevice : public DvbSubdevice
{
	private:
	
		bool waitUntilTuned(int timeout);
	
	public:
		
		FeStatus* status;
		FeInfo* info;
		FeParams* params;
		FeBer* ber;
		FeSnr* snr;
		FeSignalStrength* signal_strength;
		
		FeDevice();
		FeDevice(int _adapter_num, int _subdevice_num);
		bool tune(	int timeout,
					int _frequency,
					fe_bandwidth_t _bandwidth,
					fe_code_rate_t _code_rate_HP,
					fe_code_rate_t _code_rate_LP,
					fe_spectral_inversion_t _inversion,
					fe_modulation_t _constellation,
					fe_transmit_mode_t _transmission_mode,
					fe_guard_interval_t _guard_interval,
					fe_hierarchy_t _hierarchy_information);
		void readBer();
		void readSnr();
		void readSignalStrength();
		void print();
		~FeDevice();
};

class DmxDevice : public DvbSubdevice
{
	public:
		
		DmxPesFilter *pes_filter;
		DmxBufferSize *buffer_size;
		DmxStarter *starter;
		
		DmxDevice();
		DmxDevice(int _adapter_num, int _subdevice_num);
		bool setPesFilter	(
								int _pid,
								dmx_input_t _input,
								dmx_output_t _output,
								dmx_pes_type_t _pes_type,
								int _flags
							);
		bool setBufferSize(unsigned long _bufferSize);
		bool start();
		void print();
		~DmxDevice();
};

class DvrDevice : public DvbSubdevice
{
	private:
	public:
		
		unsigned char *buffer;
		unsigned char packet[188];
		int num_of_packets_per_read;
		int num_of_packets_read;
		int num_of_packets_serviced;
		int read_size;

		DvrDevice();
		DvrDevice(int _adapter_num, int _subdevice_num, int _num_of_packets_per_read);
		unsigned char *receivePacket();
		void print();
		~DvrDevice();
};

fe_bandwidth_t getBandwidthFromStr(std::string str);
fe_code_rate_t getCodeRateFromStr(std::string str);
fe_spectral_inversion_t getInversionFromStr(std::string str);
fe_modulation_t getConstellationFromStr(std::string str);
fe_transmit_mode_t getTransmissionModeFromStr(std::string str);
fe_guard_interval_t getGuardInterval(std::string str);
fe_hierarchy_t getHierarchyInformation(std::string str);

#endif
