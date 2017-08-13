#include <iostream>

#include "TsPes.h"
#include "debugMessagePrinter.h"

using namespace std;

TsPes::TsPes()
{
	total_length = 0;
	current_length = 0;
	has_error = false;
}

TsPes::TsPes(TsPacket *packet)
{
	total_length = 0;
	current_length = 0;
	has_error = false;
	total_length = packet->packet_payload->header->PES_packet_length;
	addPesPacket(packet);
}

void TsPes::addPesPacket(TsPacket *packet)
{
	current_length += TS_PACKET_SIZE - packet->packet_payload->pes_payload_start_offset;
	if(current_length > total_length && total_length != 0)
	{
		has_error = true;
		printDebugMessage("PES OVERFLOW " + to_str(current_length) + "/" + to_str(total_length), ERROR);
	}
}

bool TsPes::isEnded()
{
	if(total_length == 0)
	{
		printDebugMessage("completed PES packet length: " + to_str(current_length), INFO);
	}
	return (current_length == total_length) || (total_length == 0);
}

TsPes::~TsPes(){}
