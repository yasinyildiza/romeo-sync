#ifndef _FRAME_BASE_H
#define _FRAME_BASE_H

#include "debugMessagePrinter.h"

class FrameBase
{
	private:
	
		long long int arrival_time;	//us
		long long int departure_time;	//us
	
	public:
	
		FrameBase();
		void setArrivalTime();
		void setArrivalTime(long long int _arrival_time);
		long long int getArrivalTime();
		void setDepartureTime();
		void setDepartureTime(long long int _departure_time);
		long long int getDepartureTime();
		long long int getTimeOffsetAsUsec();
		long long int getTimeOffsetAsMsec();
		long long int getTimeOffsetAsSec();
		long long int getTimeOffsetAsPTS();
		~FrameBase();
};

#endif
