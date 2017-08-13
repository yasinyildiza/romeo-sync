#ifndef _DVB_OBJECTS_H
#define _DVB_OBJECTS_H

#include "frontend.h"
#include "dmx.h"

class DvbObjectBase
{
	protected:
		int fd;
	public:
	
		DvbObjectBase();
		virtual void print() = 0;
		virtual ~DvbObjectBase();
};

class FeInfo : DvbObjectBase
{
	private:

		dvb_frontend_info info;
		std::string convertFeType(int fe_type);
		
	public:
	
		std::string name;
		std::string type;
		int frequency_min;
		int frequency_max;
		int frequency_stepsize;
		int frequency_tolerance;
		int symbol_rate_min;
		int symbol_rate_max;
		int symbol_rate_tolerance;
		int notifier_delay;
		
		FeInfo();
		FeInfo(int _fd);
		void print();
		~FeInfo();
};

class FeStatus : DvbObjectBase
{
	private:

		fe_status_t status;
	
	public:
	
		bool has_signal;
		bool has_carrier;
		bool has_viterbi;
		bool has_sync;
		bool has_lock;
		bool timedout;
		bool reinit;
		
		FeStatus();
		FeStatus(int _fd);
		void reset();
		void print();
		~FeStatus();
};

class FeParams : DvbObjectBase
{
	private:

		dvb_frontend_parameters set_params;
		dvb_frontend_parameters get_params;
		
	public:
		
		int frequency;	//Hz
		fe_bandwidth_t bandwidth;
		fe_code_rate_t code_rate_HP;
		fe_code_rate_t code_rate_LP;
		fe_spectral_inversion_t inversion;
		fe_modulation_t constellation;
		fe_transmit_mode_t transmission_mode;
		fe_guard_interval_t guard_interval;
		fe_hierarchy_t hierarchy_information;
		
		FeParams();
		FeParams(	int _fd,
					int _frequency,
					fe_bandwidth_t _bandwidth,
					fe_code_rate_t _code_rate_HP,
					fe_code_rate_t _code_rate_LP,
					fe_spectral_inversion_t _inversion,
					fe_modulation_t _constellation,
					fe_transmit_mode_t _transmission_mode,
					fe_guard_interval_t _guard_interval,
					fe_hierarchy_t _hierarchy_information);
		void print();
		~FeParams();
};

class FeBer : DvbObjectBase
{
	private:
	
	public:
		
		int value;
		
		FeBer();
		FeBer(int _fd);
		void reset();
		void print();
		~FeBer();
};

class FeSnr : DvbObjectBase
{
	private:

	public:
		
		int value;
		
		FeSnr();
		FeSnr(int _fd);
		void reset();
		void print();
		~FeSnr();
};

class FeSignalStrength : DvbObjectBase
{
	private:

	public:
		
		int value;
		
		FeSignalStrength();
		FeSignalStrength(int _fd);
		void reset();
		void print();
		~FeSignalStrength();
};

class DmxPesFilter : DvbObjectBase
{
	private:
	
		dmx_pes_filter_params pes_filter;
		
	public:
		
		int pid;
		dmx_input_t input;
		dmx_output_t output;
		dmx_pes_type_t pes_type;
		int flags;
		
		DmxPesFilter();
		DmxPesFilter(	int _fd,
						int _pid,
						dmx_input_t _input,
						dmx_output_t _output,
						dmx_pes_type_t _pes_type,
						int _flags);
		void print();
		~DmxPesFilter();
};

class DmxBufferSize : DvbObjectBase
{
	private:	
		
	public:
	
		unsigned long buffer_size;
		
		DmxBufferSize();
		DmxBufferSize(int _fd, unsigned long _buffer_size);
		void print();
		~DmxBufferSize();
};

class DmxStarter : DvbObjectBase
{
	private:
		
	public:
	
		DmxStarter();
		DmxStarter(int _fd);
		void print();
		~DmxStarter();
};

#endif