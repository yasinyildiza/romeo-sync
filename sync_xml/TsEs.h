#ifndef _TS_ES_H
#define _TS_ES_H

#include "TsPacket.h"
#include "TsFrameBuffer.h"

class TsEsBase
{
	protected:
		TsFrameBase *current_frame;
		es_source_types source;
		int index;
		es_types type;
		es_subtypes subtype;

		void initializeTs(es_source_types _source, int _index, es_types _type, es_subtypes _subtype);
		virtual void initializeFrame(TsPacket *packet) = 0;
		
	public:
		TsFrameBufferBase *buffer;
		unsigned int pid;

		TsEsBase();
		virtual void addPesPacket(TsPacket *packet);
		unsigned int getFirstPTS();
		void syncByPTS(unsigned int PTS);
		void printBufferStatus();
		virtual ~TsEsBase();
};

class TsVideoEs : public TsEsBase
 {
 	public:
 		TsVideoEs(es_source_types _source, int _index, es_subtypes _subtype);
 		void initializeFrame(TsPacket *packet);
 		~TsVideoEs();
 };

 class TsAudioEs : public TsEsBase
 {
 	public:
 		TsAudioEs(es_source_types _source, int _index, es_subtypes _subtype);
 		void initializeFrame(TsPacket *packet);
 		~TsAudioEs();
 };

 class TsPrivateDataEs : public TsEsBase
 {
 	public:
 		TsPrivateDataEs(es_source_types _source, int _index, es_subtypes _subtype);
 		void initializeFrame(TsPacket *packet);
 		//void addPesPacket(TsPacket *packet);
 		~TsPrivateDataEs();
 };

#endif
