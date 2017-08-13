#include <iostream>

#include "DvbSubdevices.h"
#include "debugMessagePrinter.h"

using namespace std;

DvbSubdevice::DvbSubdevice()
{
	fd = -1;
	adapter_num = -1;
	subdevice_num = -1;
	subdevice_type = UNKNOWN;
}

DvbSubdevice::DvbSubdevice(int _adapter_num, int _subdevice_num, dvb_subdevice_type _subdevice_type)
{
	fd = -1;
	adapter_num = _adapter_num;
	subdevice_num = _subdevice_num;
	subdevice_type = _subdevice_type;
}

string DvbSubdevice::getSubDeviceName()
{
	if(subdevice_type == DVB_FRONTEND)
		return "frontend";
	if(subdevice_type == DVB_DEMUX)
		return "demux";
	if(subdevice_type == DVB_DVR)
		return "dvr";
	return "unknown";
}

void DvbSubdevice::Xopen(int flags)
{
	string path = "/dev/dvb/adapter" + to_str(adapter_num) + "/" + getSubDeviceName() + to_str(subdevice_num);
	int temp = open(path.c_str(), flags);
	if(temp >= 0)
	{
		close(temp);
		temp = open(path.c_str(), flags);
	}
	else
	{
		printDebugMessage("device could not be opened " + path, ERROR);
		return;
	}
	if(temp < 0)
	{
		printDebugMessage("device could not be opened " + path, ERROR);
		return;
	}
	fd = temp;
}

void DvbSubdevice::Xclose()
{
	if(fd < 0)
		return;
	close(fd);
}


void DvbSubdevice::openRD()
{
	Xopen(O_RDONLY);
}

void DvbSubdevice::openRDWR()
{
	Xopen(O_RDWR | O_NONBLOCK);
}

bool DvbSubdevice::isDeviceOpen()
{
	if(fd < 0)
	{
		printDebugMessage("device is not open " + getSubDeviceName(), ERROR);
		return false;
	}
	return true;
}

DvbSubdevice::~DvbSubdevice(){}

FeDevice::FeDevice()
{
	status = NULL;
	info = NULL;
	params = NULL;
	ber = NULL;
	snr = NULL;
	signal_strength = NULL;
}

FeDevice::FeDevice(int _adapter_num, int _subdevice_num) : DvbSubdevice(_adapter_num, _subdevice_num, DVB_FRONTEND)
{
	status = NULL;
	info = NULL;
	params = NULL;
	ber = NULL;
	snr = NULL;
	signal_strength = NULL;

	openRDWR();
}

bool FeDevice::tune(	int timeout,
						int _frequency,
						fe_bandwidth_t _bandwidth,
						fe_code_rate_t _code_rate_HP,
						fe_code_rate_t _code_rate_LP,
						fe_spectral_inversion_t _inversion,
						fe_modulation_t _constellation,
						fe_transmit_mode_t _transmission_mode,
						fe_guard_interval_t _guard_interval,
						fe_hierarchy_t _hierarchy_information)
{
	
	if(!isDeviceOpen())
		return false;

	params = new FeParams(	fd,
							_frequency,
							_bandwidth,
							_code_rate_HP,
							_code_rate_LP,
							_inversion,
							_constellation,
							_transmission_mode,
							_guard_interval,
							_hierarchy_information);
	status = new FeStatus(fd);
	printDebugMessage("tune started at " + now_str(), INFO);
	if(!waitUntilTuned(timeout))
	{
		printDebugMessage("tuned failed at " + now_str(), ERROR);
		return false;
	}
	printDebugMessage("tune OK at " + now_str(), INFO);
	ber = new FeBer(fd);
	snr = new FeSnr(fd);
	signal_strength = new FeSignalStrength(fd);
	return true;
}

bool FeDevice::waitUntilTuned(int timeout)
{
	status->reset();

	int tune_time = 0;
	while(!(status->has_lock))
	{
		tune_time++;
		if(tune_time == (timeout * 100))
		{
			printDebugMessage("tuning timeout...", WARNING);
			return false;
		}
		status->reset();
	}
	return true;
}

void FeDevice::readBer()
{
	ber->reset();
}

void FeDevice::readSnr()
{
	snr->reset();
}

void FeDevice::readSignalStrength()
{
	signal_strength->reset();
}

void FeDevice::print()
{
	printDebugMessage("frontend device attributes:", INFO);
	if(status != NULL)
		status->print();
	if(info != NULL)
		info->print();
	if(params != NULL)
		params->print();
	if(ber != NULL)
		ber->print();
	if(snr != NULL)
		snr->print();
	if(signal_strength != NULL)
		signal_strength->print();
}

FeDevice::~FeDevice()
{
	delete status;
	delete info;
	delete params;
	delete ber;
	delete snr;
	delete signal_strength;
}

DmxDevice::DmxDevice()
{
	pes_filter = NULL;
	buffer_size = NULL;
	starter = NULL;
}

DmxDevice::DmxDevice(int _adapter_num, int _subdevice_num) : DvbSubdevice(_adapter_num, _subdevice_num, DVB_DEMUX)
{
	pes_filter = NULL;
	buffer_size = NULL;
	starter = NULL;

	openRDWR();
}

bool DmxDevice::setPesFilter(	int _pid,
								dmx_input_t _input,
								dmx_output_t _output,
								dmx_pes_type_t _pes_type,
								int _flags)
{
	if(!isDeviceOpen())
		return false;

	pes_filter = new DmxPesFilter(fd, _pid, _input, _output, _pes_type, _flags);
	return true;
}

bool DmxDevice::setBufferSize(unsigned long _bufferSize)
{
	if(!isDeviceOpen())
		return false;

	buffer_size = new DmxBufferSize(fd, _bufferSize);
	return true;
}

bool DmxDevice::start()
{
	if(!isDeviceOpen())
		return false;

	starter = new DmxStarter(fd);
	return true;
}

void DmxDevice::print()
{
	printDebugMessage("demux device attributes:", INFO);
	if(pes_filter != NULL)
		pes_filter->print();
	if(buffer_size != NULL)
		buffer_size->print();
	if(starter != NULL)
		starter->print();
}

DmxDevice::~DmxDevice()
{
	delete pes_filter;
	delete buffer_size;
	delete starter;
}

DvrDevice::DvrDevice()
{
	num_of_packets_per_read = 0;
	num_of_packets_read = 0;
	read_size = 0;
	num_of_packets_serviced = 0;
	buffer = NULL;
}

DvrDevice::DvrDevice(int _adapter_num, int _subdevice_num, int _num_of_packets_per_read) : DvbSubdevice(_adapter_num, _subdevice_num, DVB_DVR)
{
	num_of_packets_per_read = _num_of_packets_per_read;
	num_of_packets_read = 0;
	read_size = 0;
	num_of_packets_serviced = 0;
	buffer = new unsigned char[TS_PACKET_SIZE * num_of_packets_per_read];

	openRD();
}

unsigned char *DvrDevice::receivePacket()
{
	if(errno == EOVERFLOW)
	{
		printDebugMessage("OVERFLOW in dvr buffer!", ERROR);
		return NULL;
	}

	while(read_size <= 0)
	{
		read_size = read(fd, buffer, TS_PACKET_SIZE * num_of_packets_per_read);
		/*if(read_size == 0)
		{
			usleep(1000);
			continue;
		}*/
		num_of_packets_read = read_size / TS_PACKET_SIZE;
	}

	if(num_of_packets_serviced < num_of_packets_read)
	{
		/*memcpy(packet, buffer + (num_of_packets_serviced * TS_PACKET_SIZE), TS_PACKET_SIZE);*/
		num_of_packets_serviced++;
		return buffer + ((num_of_packets_serviced - 1) * TS_PACKET_SIZE);
	}
	else
	{
		num_of_packets_serviced = 0;
		read_size = 0;
		num_of_packets_read = 0;
		return receivePacket();
	}

}

void DvrDevice::print()
{
	printDebugMessage("dvr device attributes:", INFO);
}

DvrDevice::~DvrDevice()
{
	delete []buffer;
}

fe_bandwidth_t getBandwidthFromStr(string str)
{
	if(str == "BANDWIDTH_8_MHZ")
		return BANDWIDTH_8_MHZ;
	if(str == "BANDWIDTH_7_MHZ")
		return BANDWIDTH_7_MHZ;
	if(str == "BANDWIDTH_6_MHZ")
		return BANDWIDTH_6_MHZ;
	if(str == "BANDWIDTH_AUTO")
		return BANDWIDTH_AUTO;
	if(str == "BANDWIDTH_5_MHZ")
		return BANDWIDTH_5_MHZ;
	if(str == "BANDWIDTH_10_MHZ")
		return BANDWIDTH_10_MHZ;
	if(str == "BANDWIDTH_1_712_MHZ")
		return BANDWIDTH_1_712_MHZ;
	return BANDWIDTH_AUTO;
}

fe_code_rate_t getCodeRateFromStr(string str)
{
	if(str == "FEC_NONE")
		return FEC_NONE;
	if(str == "FEC_1_2")
		return FEC_1_2;
	if(str == "FEC_2_3")
		return FEC_2_3;
	if(str == "FEC_3_4")
		return FEC_3_4;
	if(str == "FEC_4_5")
		return FEC_4_5;
	if(str == "FEC_5_6")
		return FEC_5_6;
	if(str == "FEC_6_7")
		return FEC_6_7;
	if(str == "FEC_7_8")
		return FEC_7_8;
	if(str == "FEC_8_9")
		return FEC_8_9;
	if(str == "FEC_AUTO")
		return FEC_AUTO;
	if(str == "FEC_3_5")
		return FEC_3_5;
	if(str == "FEC_9_10")
		return FEC_9_10;
	return FEC_AUTO;
}

fe_spectral_inversion_t getInversionFromStr(string str)
{
	if(str == "INVERSION_OFF")
		return INVERSION_OFF;
	if(str == "INVERSION_ON")
		return INVERSION_ON;
	if(str == "INVERSION_AUTO")
		return INVERSION_AUTO;
	return INVERSION_AUTO;
}

fe_modulation_t getConstellationFromStr(string str)
{
	if(str == "QPSK")
		return QPSK;
	if(str == "QAM_16")
		return QAM_16;
	if(str == "QAM_32")
		return QAM_32;
	if(str == "QAM_128")
		return QAM_128;
	if(str == "QAM_256")
		return QAM_256;
	if(str == "QAM_AUTO")
		return QAM_AUTO;
	if(str == "VSB_8")
		return VSB_8;
	if(str == "VSB_16")
		return VSB_16;
	if(str == "PSK_8")
		return PSK_8;
	if(str == "APSK_16")
		return APSK_16;
	if(str == "APSK_32")
		return APSK_32;
	if(str == "DQPSK")
		return DQPSK;
	return QAM_AUTO;
}

fe_transmit_mode_t getTransmissionModeFromStr(string str)
{
	if(str == "TRANSMISSION_MODE_2K")
		return TRANSMISSION_MODE_2K;
	if(str == "TRANSMISSION_MODE_8K")
		return TRANSMISSION_MODE_8K;
	if(str == "TRANSMISSION_MODE_AUTO")
		return TRANSMISSION_MODE_AUTO;
	if(str == "TRANSMISSION_MODE_4K")
		return TRANSMISSION_MODE_4K;
	if(str == "TRANSMISSION_MODE_1K")
		return TRANSMISSION_MODE_1K;
	if(str == "TRANSMISSION_MODE_16K")
		return TRANSMISSION_MODE_16K;
	if(str == "TRANSMISSION_MODE_32K")
		return TRANSMISSION_MODE_32K;
	return TRANSMISSION_MODE_AUTO;
}

fe_guard_interval_t getGuardInterval(string str)
{
	if(str == "GUARD_INTERVAL_1_32")
		return GUARD_INTERVAL_1_32;
	if(str == "GUARD_INTERVAL_1_16")
		return GUARD_INTERVAL_1_16;
	if(str == "GUARD_INTERVAL_1_8")
		return GUARD_INTERVAL_1_8;
	if(str == "GUARD_INTERVAL_1_4")
		return GUARD_INTERVAL_1_4;
	if(str == "GUARD_INTERVAL_AUTO")
		return GUARD_INTERVAL_AUTO;
	if(str == "GUARD_INTERVAL_1_128")
		return GUARD_INTERVAL_1_128;
	if(str == "GUARD_INTERVAL_19_128")
		return GUARD_INTERVAL_19_128;
	if(str == "GUARD_INTERVAL_19_256")
		return GUARD_INTERVAL_19_256;
	return GUARD_INTERVAL_AUTO;
}

fe_hierarchy_t getHierarchyInformation(string str)
{
	if(str == "HIERARCHY_NONE")
		return HIERARCHY_NONE;
	if(str == "HIERARCHY_1")
		return HIERARCHY_1;
	if(str == "HIERARCHY_2")
		return HIERARCHY_2;
	if(str == "HIERARCHY_4")
		return HIERARCHY_4;
	if(str == "HIERARCHY_AUTO")
		return HIERARCHY_AUTO;
	return HIERARCHY_AUTO;
}
