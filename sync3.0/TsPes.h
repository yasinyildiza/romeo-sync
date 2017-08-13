#ifndef _TS_PES_H
#define _TS_PES_H

#include "TsPacket.h"

class TsPes
{
	private:

		unsigned short int total_length;
		unsigned short int current_length;
		
	public:

		bool has_error;

		TsPes();
		TsPes(TsPacket *packet);
		void addPesPacket(TsPacket *packet);
		bool isEnded();
		~TsPes();
};

#endif