#ifndef _TS_PACKET_BUFFER_H
#define _TS_PACKET_BUFFER_H

#include "TsPacketContainer.h"
#include "BufferBase.h"

class TsPacketBuffer : public BufferBase<TsPacketContainer>
{
	private:
	public:
		TsPacketBuffer(std::string _buffer_id, int _total_size);
		void insertNewNode(TsPacketContainer *packet);
		~TsPacketBuffer();
};

#endif