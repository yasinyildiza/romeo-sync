#include <iostream>

#include "TsPacketBuffer.h"
#include "debugMessagePrinter.h"

using namespace std;

TsPacketBuffer::TsPacketBuffer(std::string _buffer_id, int _total_size)
{
	initialize(_buffer_id, _total_size, 10, 90);
}

void TsPacketBuffer::insertNewNode(TsPacketContainer *packet)
{
	insertTail(packet);
}

TsPacketBuffer::~TsPacketBuffer(){}