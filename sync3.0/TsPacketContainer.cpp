#include <iostream>

#include "TsPacketContainer.h"
#include "debugMessagePrinter.h"

using namespace std;

TsPacketContainer::TsPacketContainer(unsigned char *_data)
{
	memcpy(data, _data, 188);
	size = 188;
}

TsPacketContainer::~TsPacketContainer(){}