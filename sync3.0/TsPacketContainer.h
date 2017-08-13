#ifndef _TS_PACKET_CONTAINER_H
#define _TS_PACKET_CONTAINER_H

class TsPacketContainer
{
private:
public:
	unsigned char data[188];
	int size;

	TsPacketContainer(unsigned char *_data);
	~TsPacketContainer();
};

#endif