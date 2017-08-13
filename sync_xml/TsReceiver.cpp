#include <iostream>

#include "TsReceiver.h"
#include "debugMessagePrinter.h"

using namespace std;

TsReceiverBase::TsReceiverBase()
{
	source = ES_SOURCE_DVB;
	index = 0;
	type = ES_ALL;
	subtype = ES_BASE;
	conf_file = NULL;
	sending_frame_flag = true;
	receive_ts_packets_thread = NULL;
	prepare_ts_frames_thread = NULL;
	prepare_ts_enhanced_frames_thread = NULL;
	send_ts_enhanced_frames_thread = NULL;
	read_measurement_values_thread = NULL;
	receiver_id = "0000";

	ts_packet_buffer = new unsigned char[TS_PACKET_BUFFER_SIZE_AS_BYTES];
	write_offset = 0;
	read_offset = 0;
	ts_packet_buffer_current_size = 0;
	mutex = PTHREAD_MUTEX_INITIALIZER;
	ts_parser = NULL;
	is_sending = false;

	pthread_mutex_init(&count_lock, NULL);
	pthread_cond_init(&count_nonzero, NULL);
	count = 0;
	pthread_mutex_init(&count_lock2, NULL);
	pthread_cond_init(&count_nonzero2, NULL);
	count2 = 0;
	
	pthread_mutex_init(&buffer_lock, NULL);
	pthread_cond_init(&buffer_not_empty, NULL);
	buffer_count = 0;
}

void TsReceiverBase::initialize(es_source_types _source, int _index, es_types _type, es_subtypes _subtype)
{
	source = _source;
	index = _index;
	type = _type;
	subtype = _subtype;
	receiver_id = "r" + to_str(source) + to_str(index) + to_str(type) + to_str(subtype);
	//printDebugMessage("receiver " + receiver_id + " created", INFO);
}

void TsReceiverBase::lock()
{
	pthread_mutex_lock(&mutex);
}
void TsReceiverBase::release()
{
	pthread_mutex_unlock(&mutex);
}

void TsReceiverBase::tuned()
{
	start2receiveTsPackets();
	start2prepareTsFrames();
}

void TsReceiverBase::start2receiveTsPackets()
{
	if(ts_parser != NULL)
	{
		receive_ts_packets_thread = new ReceiveTsPacketsThread(this);
		receive_ts_packets_thread->execute();
		if(print_measurement_values_flag)
		{
			read_measurement_values_thread = new ReadMeasurementValuesThread(this);
			read_measurement_values_thread->execute();
		}
	}
}

void TsReceiverBase::start2prepareTsFrames()
{
	if(ts_parser != NULL)
	{
		prepare_ts_frames_thread = new PrepareTsFramesThread(this);
		prepare_ts_frames_thread->execute();
	}
}

void TsReceiverBase::start2prepareTsEnhancedFrames()
{
	if(ts_parser != NULL)
	{
		prepare_ts_enhanced_frames_thread = new PrepareTsEnhancedFramesThread(this);
		prepare_ts_enhanced_frames_thread->execute();
	}
}

void TsReceiverBase::start2sendTsEnhancedFrames()
{
	if(ts_parser != NULL)
	{
		send_ts_enhanced_frames_thread = new SendTsEnhancedFramesThread(this);
		send_ts_enhanced_frames_thread->execute();
	}
}

void TsReceiverBase::writePacket()
{
	if(ts_packet_buffer_current_size != TS_PACKET_BUFFER_SIZE_AS_PACKETS)
	{
		packet = receivePacket();
		if(packet != NULL)
		{
			memcpy(ts_packet_buffer+write_offset, packet, TS_PACKET_SIZE);
			lock();
			/*if(ts_packet_buffer_current_size == 0)
			{
				pthread_mutex_lock(&buffer_lock);
				if(buffer_count == 0)
					pthread_cond_signal(&buffer_not_empty);
				buffer_count++;
				pthread_mutex_unlock(&buffer_lock);
			}*/
			ts_packet_buffer_current_size++;
			release();
			write_offset = (write_offset + TS_PACKET_SIZE) % TS_PACKET_BUFFER_SIZE_AS_BYTES;
		}
		else
		{
			stopThreads();
		}
	}
	else
	{
		writeDebugMessage(receiver_id + "_error.txt", "full " + now_str(), ERROR);
	}
}

void TsReceiverBase::readPacket()
{
	if(ts_packet_buffer_current_size == 0)
	{
		/*printDebugMessage(receiver_id + " ts packet buffer is empty", STATUS);
		pthread_mutex_lock(&buffer_lock);
		while(buffer_count == 0)
			pthread_cond_wait(&buffer_not_empty, &mutex);
		buffer_count--;
		pthread_mutex_unlock(&buffer_lock);*/
		usleep(100);
	}
	else
	{
		lock();
		ts_packet_buffer_current_size--;
		release();
		ts_parser->parse(ts_packet_buffer+read_offset);
		read_offset = (read_offset + TS_PACKET_SIZE) % TS_PACKET_BUFFER_SIZE_AS_BYTES;
	}
}

unsigned int TsReceiverBase::getFirstPTS()
{
	unsigned int first_pts = (ts_parser == NULL) ? 0 : ts_parser->getFirstPTS();
	printDebugMessage(receiver_id + " first PTS: " + to_str(first_pts), INFO);
	return first_pts;
}

void TsReceiverBase::syncByPTS(unsigned int PTS)
{
	if(ts_parser != NULL) ts_parser->syncByPTS(PTS);
}

void TsReceiverBase::prepareFrame()
{
	pthread_mutex_lock(&count_lock);
    while(count == 0)
        pthread_cond_wait(&count_nonzero, &count_lock);
    count = count - 1;
    ts_parser->prepareFrame();
    pthread_mutex_unlock(&count_lock);
}

void TsReceiverBase::sendFrame()
{
	pthread_mutex_lock(&count_lock2);
    while(count2 == 0)
        pthread_cond_wait(&count_nonzero2, &count_lock2);
    count2 = count2 - 1;
    ts_parser->sendFrame();
    pthread_mutex_unlock(&count_lock2);
}

void TsReceiverBase::printBufferStatus()
{
	if(ts_parser != NULL) ts_parser->printBufferStatus();
}

void TsReceiverBase::stopThreads()
{
	if(receive_ts_packets_thread != NULL)
	{
		printDebugMessage("receiver " + receiver_id + " ReceiveTsPacketsThread is stopping...", STATUS);
		receive_ts_packets_thread->stop();
		printDebugMessage("receiver " + receiver_id + " ReceiveTsPacketsThread is stopped...", STATUS);
	}
	if(prepare_ts_frames_thread != NULL)
	{
		printDebugMessage("receiver " + receiver_id + " PrepareTsFramesThread is stopping...", STATUS);
		prepare_ts_frames_thread->stop();
		printDebugMessage("receiver " + receiver_id + " PrepareTsFramesThread is stopped...", STATUS);
	}
	if(prepare_ts_enhanced_frames_thread != NULL)
	{
		printDebugMessage("receiver " + receiver_id + " PrepareTsEnhancedFramesThread is stopping...", STATUS);
		prepare_ts_enhanced_frames_thread->stop();
		printDebugMessage("receiver " + receiver_id + " PrepareTsEnhancedFramesThread is stopped...", STATUS);
	}
	if(send_ts_enhanced_frames_thread != NULL)
	{
		printDebugMessage("receiver " + receiver_id + " SendTsEnhancedFramesThread is stopping...", STATUS);
		send_ts_enhanced_frames_thread->stop();
		printDebugMessage("receiver " + receiver_id + " SendTsEnhancedFramesThread is stopped...", STATUS);
	}
	if(read_measurement_values_thread != NULL)
	{
		printDebugMessage("receiver " + receiver_id + " ReadMeasurementValuesThread is stopping...", STATUS);
		read_measurement_values_thread->stop();
		printDebugMessage("receiver " + receiver_id + " ReadMeasurementValuesThread is stopped...", STATUS);
	}
}

void TsReceiverBase::waitForAllowSend()
{
	while(true)
	{
		lock();
		if(is_sending)
		{
			release();
			break;
		}
		release();
	}
}

void TsReceiverBase::waitForSendComplete()
{
	while(true)
	{
		lock();
		if(!is_sending)
		{
			release();
			break;
		}
		release();
	}
}

void TsReceiverBase::allowSend()
{
	lock();
	is_sending = true;
	release();
}

void TsReceiverBase::sendComplete()
{
	lock();
	is_sending = false;
	release();
}

void TsReceiverBase::readMeasurementValues()
{}

void TsReceiverBase::printMeasurementValues()
{}

TsReceiverBase::~TsReceiverBase()
{
	delete conf_file;
	delete []ts_packet_buffer;
	delete receive_ts_packets_thread;
	delete prepare_ts_frames_thread;
	delete prepare_ts_enhanced_frames_thread;
	delete send_ts_enhanced_frames_thread;
	delete read_measurement_values_thread;
}

ReceiveTsPacketsThread::ReceiveTsPacketsThread(TsReceiverBase *_receiver)
{
	receiver = _receiver;
}

void ReceiveTsPacketsThread::run()
{
	//printDebugMessage("receiver " + receiver->receiver_id + " ReceiveTsPacketsThread started at " + now_str(), STATUS);
	while(true)
	{
		receiver->writePacket();
		//pthread_testcancel();
	}
	//printDebugMessage("receiver " + receiver->receiver_id + " ReceiveTsPacketsThread stopped", STATUS);
}

ReceiveTsPacketsThread::~ReceiveTsPacketsThread(){}

PrepareTsFramesThread::PrepareTsFramesThread(TsReceiverBase *_receiver)
{
	receiver = _receiver;
}

void PrepareTsFramesThread::run()
{
	//printDebugMessage("receiver " + receiver->receiver_id + " PrepareTsFramesThread started at " + now_str(), STATUS);
	while(true)
	{
		receiver->readPacket();
		//pthread_testcancel();
	}
	//printDebugMessage("receiver " + receiver->receiver_id + " PrepareTsFramesThread stopped", STATUS);;
}

PrepareTsFramesThread::~PrepareTsFramesThread(){}

PrepareTsEnhancedFramesThread::PrepareTsEnhancedFramesThread(TsReceiverBase *_receiver)
{
	receiver = _receiver;
}

void PrepareTsEnhancedFramesThread::run()
{
	//printDebugMessage("receiver " + receiver->receiver_id + " PrepareTsEnhancedFramesThread started at " + now_str(), STATUS);
	while(true)
	{
		receiver->prepareFrame();
		//pthread_testcancel();
	}
	//printDebugMessage("receiver " + receiver->receiver_id + " PrepareTsEnhancedFramesThread stopped", STATUS);
}

PrepareTsEnhancedFramesThread::~PrepareTsEnhancedFramesThread(){}

SendTsEnhancedFramesThread::SendTsEnhancedFramesThread(TsReceiverBase *_receiver)
{
	receiver = _receiver;
}

void SendTsEnhancedFramesThread::run()
{
	usleep((prebuffering_as_seconds * ONE_SECOND) / 2);
	//printDebugMessage("receiver " + receiver->receiver_id + " SendTsEnhancedFramesThread started at " + now_str(), STATUS);
	while(true)
	{
		receiver->sendFrame();
		//pthread_testcancel();
	}
	//printDebugMessage("receiver " + receiver->receiver_id + " SendTsEnhancedFramesThread stopped", STATUS);
}

SendTsEnhancedFramesThread::~SendTsEnhancedFramesThread(){}

ReadMeasurementValuesThread::ReadMeasurementValuesThread(TsReceiverBase *_receiver)
{
	receiver = _receiver;
}

void ReadMeasurementValuesThread::run()
{
	//printDebugMessage("receiver " + receiver->receiver_id + " ReadMeasurementValuesThread started at " + now_str(), STATUS);
	while(true)
	{
		receiver->readMeasurementValues();
		receiver->printMeasurementValues();
		usleep(print_measurement_values_period_as_seconds * ONE_SECOND);
		pthread_testcancel();
	}
	//printDebugMessage("receiver " + receiver->receiver_id + " ReadMeasurementValuesThread stopped", STATUS);
}

ReadMeasurementValuesThread::~ReadMeasurementValuesThread(){}

TsReceiverDVB::TsReceiverDVB()
{
	configure();
}

TsReceiverDVB::TsReceiverDVB(	int _frequency,
								fe_bandwidth_t _bandwidth,
								fe_code_rate_t _code_rate_HP,
								fe_code_rate_t _code_rate_LP,
								fe_spectral_inversion_t _inversion,
								fe_modulation_t _constellation,
								fe_transmit_mode_t _transmission_mode,
								fe_guard_interval_t _guard_interval,
								fe_hierarchy_t _hierarchy_information)
{
	configure();

	frequency = _frequency;
	bandwidth = _bandwidth;
	code_rate_HP = _code_rate_HP;
	code_rate_LP = _code_rate_LP;
	inversion = _inversion;
	constellation = _constellation;
	transmission_mode = _transmission_mode;
	guard_interval = _guard_interval;
	hierarchy_information = _hierarchy_information;
}

void TsReceiverDVB::configure()
{
	initialize(ES_SOURCE_DVB, 0, ES_ALL, ES_BASE);

	fe = NULL;
	dmx = NULL;
	dvr = NULL;

	conf_file = new ConfigFileParser("dvb_reception.conf", ':');
	
	adapter_num = conf_file->getIntValueByName("adapter_num");
	fe_num = conf_file->getIntValueByName("fe_num");
	dmx_num = conf_file->getIntValueByName("dmx_num");
	dvr_num = conf_file->getIntValueByName("dvr_num");
	num_of_packets_per_read = conf_file->getIntValueByName("num_of_packets_per_read");
	
	tune_timeout = conf_file->getIntValueByName("tune_timeout");
	frequency = conf_file->getIntValueByName("frequency") * 1000;
	bandwidth = getBandwidthFromStr(conf_file->getStrValueByName("bandwidth"));
	code_rate_HP = getCodeRateFromStr(conf_file->getStrValueByName("code_rate_HP"));
	code_rate_LP = getCodeRateFromStr(conf_file->getStrValueByName("code_rate_LP"));
	inversion = getInversionFromStr(conf_file->getStrValueByName("inversion"));
	constellation = getConstellationFromStr(conf_file->getStrValueByName("constellation"));
	transmission_mode = getTransmissionModeFromStr(conf_file->getStrValueByName("transmission_mode"));
	guard_interval = getGuardInterval(conf_file->getStrValueByName("guard_interval"));
	hierarchy_information = getHierarchyInformation(conf_file->getStrValueByName("hierarchy_information"));
}

bool TsReceiverDVB::tune(TsParserBase *parser)
{
	fe = new FeDevice(adapter_num, fe_num);
	dvr = new DvrDevice(adapter_num, dvr_num, num_of_packets_per_read);
	dmx = new DmxDevice(adapter_num, dmx_num);

	if(fe->tune(tune_timeout, frequency, bandwidth, code_rate_HP, code_rate_LP, inversion, constellation, transmission_mode, guard_interval, hierarchy_information))
	{
		if(dmx->setPesFilter(0x2000, DMX_IN_FRONTEND, DMX_OUT_TS_TAP, DMX_PES_OTHER, DMX_IMMEDIATE_START))
		{
			ts_parser = new TsParserDVB();
			tuned();
			return true;
		}
	}
	return false;
}

unsigned char *TsReceiverDVB::receivePacket()
{
	return dvr->receivePacket();
}

void TsReceiverDVB::readMeasurementValues()
{
	fe->readBer();
	fe->readSnr();
	fe->readSignalStrength();
}

void TsReceiverDVB::printMeasurementValues()
{
	printDebugMessage("receiver " + receiver_id + "	measurement values:", INFO);
	printDebugMessage("\tbit error rate: " + to_str(fe->ber->value), INFO);
	printDebugMessage("\tsignal to noise ratio: " + to_str(fe->snr->value), INFO);
	printDebugMessage("\tsignal strength: " + to_str(fe->signal_strength->value), INFO);
}

void TsReceiverDVB::stop()
{
	if(fe != NULL) fe->Xclose();
	if(dmx != NULL)	dmx->Xclose();
	if(dvr != NULL)	dvr->Xclose();
}

TsReceiverDVB::~TsReceiverDVB()
{
	delete fe;
	delete dmx;
	delete dvr;
	delete ts_parser;
}

TsReceiverP2PBase::TsReceiverP2PBase()
{
	num_of_packets_per_read = 0;
	buffer = NULL;
	num_of_packets_read = 0;
	num_of_packets_serviced = 0;
	read_size = 0;
	packet_receiver = NULL;
	receiver_port_number = 0;
	es_pid = 0x2000;

	configure();
}

void TsReceiverP2PBase::configure()
{
	conf_file = new ConfigFileParser("p2p_reception.conf", ':');
	num_of_packets_per_read = conf_file->getIntValueByName("num_of_packets_per_read");
}

void TsReceiverP2PBase::initializeTs(unsigned short int pid, int port_number)
{
	es_pid = pid;
	receiver_port_number = port_number;
	packet_receiver = new UDPserver(receiver_port_number);
	printDebugMessage("receiving pid " + to_str(es_pid) + " from P2P port: " + to_str(receiver_port_number), INFO);
}

unsigned char *TsReceiverP2PBase::receivePacket()
{
	while(read_size <= 0)
	{
		buffer = new unsigned char[TS_PACKET_SIZE * num_of_packets_per_read];
		packet_receiver->recv(buffer, TS_PACKET_SIZE * num_of_packets_per_read);
		read_size = packet_receiver->getLastMessageLength();
		num_of_packets_read = read_size / TS_PACKET_SIZE;
	}

	if(num_of_packets_serviced < num_of_packets_read)
	{
		/*memcpy(packet, buffer + (num_of_packets_serviced * TS_PACKET_SIZE), TS_PACKET_SIZE);*/
		num_of_packets_serviced++;
		return buffer + ((num_of_packets_serviced - 1) * TS_PACKET_SIZE);
	}
	else
	{
		delete []buffer;
		buffer = NULL;
		num_of_packets_serviced = 0;
		read_size = 0;
		return receivePacket();
	}
}

void TsReceiverP2PBase::stop(){}

TsReceiverP2PBase::~TsReceiverP2PBase()
{
	delete []buffer;
	delete packet_receiver;
}

TsReceiverP2PVideoBase::TsReceiverP2PVideoBase(int _index)
{
	initialize(ES_SOURCE_P2P, _index, ES_VIDEO, ES_BASE);
	initializeTs(video_pid_base + index * 2, conf_file->getIntValueByName("video_receiver_port_base") + index * 2);
}

bool TsReceiverP2PVideoBase::tune(TsParserBase *parser)
{
	ts_parser = new TsParserP2PVideoBase(index, es_pid);
	tuned();
	return true;
}

TsReceiverP2PVideoBase::~TsReceiverP2PVideoBase()
{
	delete ts_parser;
}

TsReceiverP2PVideoEnhancement::TsReceiverP2PVideoEnhancement(int _index)
{
	initialize(ES_SOURCE_P2P, _index, ES_VIDEO, ES_ENHANCEMENT);
	initializeTs(video_pid_base + index * 2 + 1, conf_file->getIntValueByName("video_receiver_port_base") + index * 2 + 1);
	sending_frame_flag = false;
}

bool TsReceiverP2PVideoEnhancement::tune(TsParserBase *parser)
{
	ts_parser = new TsParserP2PVideoEnhancement(parser);
	tuned();
	return true;
}

void TsReceiverP2PVideoEnhancement::start2prepareTsEnhancedFrames(){}
//void TsReceiverP2PVideoEnhancement::start2sendTsEnhancedFrames(){}
void TsReceiverP2PVideoEnhancement::syncByPTS(unsigned int PTS){}
void TsReceiverP2PVideoEnhancement::sendFrame(){}
void TsReceiverP2PVideoEnhancement::printBufferStatus(){}

TsReceiverP2PVideoEnhancement::~TsReceiverP2PVideoEnhancement()
{
	delete ts_parser;
}

TsReceiverP2PAudio::TsReceiverP2PAudio(int _index)
{
	initialize(ES_SOURCE_P2P, _index, ES_AUDIO, ES_BASE);
	initializeTs(audio_pid_base + index*2, conf_file->getIntValueByName("audio_receiver_port_base") + index);
}

bool TsReceiverP2PAudio::tune(TsParserBase *parser)
{
	ts_parser = new TsParserP2PAudio(index, es_pid);
	tuned();
	return true;
}

TsReceiverP2PAudio::~TsReceiverP2PAudio()
{
	delete ts_parser;
}

TsReceiverP2PPrivateData::TsReceiverP2PPrivateData(int _index)
{
	initialize(ES_SOURCE_P2P, _index, ES_PRIVATE_DATA, ES_BASE);
	initializeTs(private_data_pid_base + index, conf_file->getIntValueByName("private_data_receiver_port_base") + index);
}

bool TsReceiverP2PPrivateData::tune(TsParserBase *parser)
{
	ts_parser = new TsParserP2PPrivateData(index, es_pid);
	tuned();
	return true;
}

TsReceiverP2PPrivateData::~TsReceiverP2PPrivateData()
{
	delete ts_parser;
}

TsReceiverTsFile::TsReceiverTsFile(){}

TsReceiverTsFile::TsReceiverTsFile(std::string _file_path)
{
	initialize(ES_SOURCE_DVB, 1, ES_ALL, ES_BASE);
	file_path = _file_path;
}

bool TsReceiverTsFile::tune(TsParserBase *parser)
{
	file_stream.open(file_path.c_str());
	if(file_stream.good())
	{
		ts_parser = new TsParserDVB();
		tuned();
		return true;
	}
	printDebugMessage("file could not be opened " + file_path, ERROR);
	return false;
}

unsigned char *TsReceiverTsFile::receivePacket()
{
	file_stream.read(packet, TS_PACKET_SIZE);
	return (unsigned char*)packet;
}

void TsReceiverTsFile::stop()
{
	file_stream.close();
}

void TsReceiverTsFile::readMeasurementValues(){}

void TsReceiverTsFile::printMeasurementValues(){}

TsReceiverTsFile::~TsReceiverTsFile()
{
	delete ts_parser;
}
