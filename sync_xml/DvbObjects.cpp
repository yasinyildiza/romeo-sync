#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>

#include "DvbObjects.h"
#include "debugMessagePrinter.h"

using namespace std;

DvbObjectBase::DvbObjectBase(){}

DvbObjectBase::~DvbObjectBase(){}

FeInfo::FeInfo(){}

FeInfo::FeInfo(int _fd)
{
	memset(&info, 0, sizeof(info));

	if (ioctl(fd, FE_GET_INFO, &info) != 0)
	{
		printDebugMessage("could not read frontend info", WARNING);
		return;
	}

	name = info.name;
	type = convertFeType(info.type);
	frequency_min = info.frequency_min;
	frequency_max = info.frequency_max;
	frequency_stepsize = info.frequency_stepsize;
	frequency_tolerance = info.frequency_tolerance;
	symbol_rate_min = info.symbol_rate_min;
	symbol_rate_max = info.symbol_rate_max;
	symbol_rate_tolerance = info.symbol_rate_tolerance;
	notifier_delay = info.notifier_delay;
}

string FeInfo::convertFeType(int fe_type)
{
	if(fe_type == 0)
		return "DVB-C";
	if(fe_type == 1)
		return "DVB-S";
	if(fe_type == 2)
		return "DVB-T";
	if(fe_type == 3)
		return "ATSC";
	return "UNKNOWN";
}

void FeInfo::print()
{
	printDebugMessage("frontend info attributes:", INFO);
	printAttribute("name", name);
	printAttribute("type", type);
	printAttribute("frequency_min", frequency_min);
	printAttribute("frequency_max", frequency_max);
	printAttribute("frequency_stepsize", frequency_stepsize);
	printAttribute("frequency_tolerance", frequency_tolerance);
	printAttribute("symbol_rate_min", symbol_rate_min);
	printAttribute("symbol_rate_max", symbol_rate_max);
	printAttribute("notifier_delay", notifier_delay);
}

FeInfo::~FeInfo(){}

FeStatus::FeStatus(){}

FeStatus::FeStatus(int _fd)
{
	fd = _fd;
	has_signal = false;
	has_carrier = false;
	has_viterbi = false;
	has_sync = false;
	has_lock = false;
	timedout = false;
	reinit = false;

	reset();
}

void FeStatus::reset()
{
	memset(&status, 0, sizeof(status));
	if (ioctl(fd, FE_READ_STATUS, &status) != 0)
	{
		printDebugMessage("could not read frontend status", WARNING);
		return;
	}
	has_signal = status & FE_HAS_SIGNAL;
	has_carrier = status & FE_HAS_CARRIER;
	has_viterbi = status & FE_HAS_VITERBI;
	has_sync = status & FE_HAS_SYNC;
	has_lock = status & FE_HAS_LOCK;
	timedout = status & FE_TIMEDOUT;
	reinit = status & FE_REINIT;
}

void FeStatus::print()
{
	printDebugMessage("frontend status attributes:", INFO);
	printAttribute("has_signal", has_signal);
	printAttribute("has_carrier", has_carrier);
	printAttribute("has_viterbi", has_viterbi);
	printAttribute("has_sync", has_sync);
	printAttribute("has_lock", has_lock);
	printAttribute("timedout", timedout);
	printAttribute("reinit", reinit);
}

FeStatus::~FeStatus(){}

FeParams::FeParams(	int _fd,
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
	fd = _fd;
	memset(&set_params, 0, sizeof(set_params));
	set_params.frequency = _frequency;
	set_params.u.ofdm.bandwidth = _bandwidth;
	set_params.u.ofdm.code_rate_HP = _code_rate_HP;
	set_params.u.ofdm.code_rate_LP = _code_rate_LP;
	set_params.inversion = _inversion;
	set_params.u.ofdm.constellation = _constellation;
	set_params.u.ofdm.transmission_mode = _transmission_mode;
	set_params.u.ofdm.guard_interval = _guard_interval;
	set_params.u.ofdm.hierarchy_information = _hierarchy_information;

	if (ioctl(fd, FE_SET_FRONTEND, &set_params) != 0)
	{
		printDebugMessage("could not set frontend params", WARNING);
		return;
	}

	memset(&get_params, 0, sizeof(get_params));
	if (ioctl(fd, FE_GET_FRONTEND, &get_params) != 0)
	{
		printDebugMessage("could not get frontend params", WARNING);
		return;
	}
	frequency = get_params.frequency;
	bandwidth = get_params.u.ofdm.bandwidth;
	code_rate_HP = get_params.u.ofdm.code_rate_HP;
	code_rate_LP = get_params.u.ofdm.code_rate_LP;
	inversion = get_params.inversion;
	constellation = get_params.u.ofdm.constellation;
	transmission_mode = get_params.u.ofdm.transmission_mode;
	guard_interval = get_params.u.ofdm.guard_interval;
	hierarchy_information = get_params.u.ofdm.hierarchy_information;
}

void FeParams::print()
{
	printDebugMessage("frontend params attributes:", INFO);
	printAttribute("frequency", frequency);
	printAttribute("bandwidth", bandwidth);
	printAttribute("code_rate_HP", code_rate_HP);
	printAttribute("code_rate_LP", code_rate_LP);
	printAttribute("inversion", inversion);
	printAttribute("constellation", constellation);
	printAttribute("transmission_mode", transmission_mode);
	printAttribute("guard_interval", guard_interval);
	printAttribute("hierarchy_information", hierarchy_information);
}

FeParams::~FeParams(){}

FeBer::FeBer(){}

FeBer::FeBer(int _fd)
{
	fd = _fd;
	memset(&value, 0, sizeof(value));
	reset();
}

void FeBer::reset()
{
	if (ioctl(fd, FE_READ_BER, &value) != 0)
	{
		printDebugMessage("could not read frontend ber", WARNING);
		return;
	}
}

void FeBer::print()
{
	printDebugMessage("frontend bit error rate attributes:", INFO);
	printAttribute("ber", value);
}

FeBer::~FeBer(){}

FeSnr::FeSnr(){}

FeSnr::FeSnr(int _fd)
{
	fd = _fd;
	memset(&value, 0, sizeof(value));
	reset();
}

void FeSnr::reset()
{
	if (ioctl(fd, FE_READ_SNR, &value) != 0)
	{
		printDebugMessage("could not read frontend snr", WARNING);
		return;
	}
}

void FeSnr::print()
{
	printDebugMessage("frontend signal to noise rate attributes:", INFO);
	printAttribute("snr", value);
}

FeSnr::~FeSnr(){}

FeSignalStrength::FeSignalStrength(){}

FeSignalStrength::FeSignalStrength(int _fd)
{
	fd = _fd;
	memset(&value, 0, sizeof(value));
	reset();
}

void FeSignalStrength::reset()
{
	if (ioctl(fd, FE_READ_SIGNAL_STRENGTH, &value) != 0)
	{
		printDebugMessage("could not read frontend signal strength", WARNING);
		return;
	}
}

void FeSignalStrength::print()
{
	printDebugMessage("frontend signal strength attributes:", INFO);
	printAttribute("signal_strength", value);
}

FeSignalStrength::~FeSignalStrength(){}

DmxPesFilter::DmxPesFilter(){}

DmxPesFilter::DmxPesFilter(	int _fd,
							int _pid,
							dmx_input_t _input,
							dmx_output_t _output,
							dmx_pes_type_t _pes_type,
							int _flags)
{
	fd = _fd;
	memset(&pes_filter, 0, sizeof(pes_filter));
	pes_filter.pid = _pid;
	pes_filter.input = _input;
	pes_filter.output = _output;
	pes_filter.pes_type = _pes_type;
	pes_filter.flags = _flags;
	if (ioctl(fd, DMX_SET_PES_FILTER, &pes_filter) != 0)
	{
		printDebugMessage("cannot set demux pes filter", WARNING);
		return;
	}
	pid = pes_filter.pid;
	input = pes_filter.input;
	output = pes_filter.output;
	pes_type = pes_filter.pes_type;
	flags = pes_filter.flags;
}

void DmxPesFilter::print()
{
	printDebugMessage("demux pes filter attributes:", INFO);
	printAttribute("pid", pid);
	printAttribute("input", input);
	printAttribute("output", output);
	printAttribute("pes_type", pes_type);
	printAttribute("flags", flags);
}

DmxPesFilter::~DmxPesFilter(){}

DmxBufferSize::DmxBufferSize(){}

DmxBufferSize::DmxBufferSize(int _fd, unsigned long _buffer_size)
{
	fd = _fd;
	if(ioctl(fd, DMX_SET_BUFFER_SIZE, buffer_size) != 0)
	{
		printDebugMessage("cannot set demux buffer size", STATUS);
		buffer_size = 2 * 4096;	// default buffer size
		return;
	}
	buffer_size = _buffer_size;
}

void DmxBufferSize::print()
{
	printDebugMessage("demux buffer size attributes:", INFO);
	printAttribute("buffer_size", buffer_size);
}

DmxBufferSize::~DmxBufferSize(){}

DmxStarter::DmxStarter(){}

DmxStarter::DmxStarter(int _fd)
{
	fd = _fd;
	if(ioctl(fd, DMX_START) != 0)
	{
		printDebugMessage("cannot start demux", WARNING);
		return;
	}
}

void DmxStarter::print()
{
	printDebugMessage("demux starter attributes:", INFO);
}

DmxStarter::~DmxStarter(){}