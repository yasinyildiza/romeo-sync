#include <iostream>

#include "TsParser.h"
#include "debugMessagePrinter.h"

unsigned char PAT_PCKT[188] = {0x47, 0x40, 0x0, 0x18, 0x0, 0x0, 0xb0, 0x11, 0x7, 0xcf, 0xc1, 0x0, 0x0, 0x0, 0x0, 0xe0, 0x10, 0x0, 0x1, 0xe0, 0x20, 0x66, 0xa9, 0xa1, 0x15, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char PMT_PCKT[188] = {0x47, 0x40, 0x20, 0x13, 0x0, 0x2, 0xb0, 0x27, 0x0, 0x1, 0xc1, 0x0, 0x0, 0xe3, 0xe9, 0xf0, 0x0, 0x1b, 0xe3, 0xe9, 0xf0, 0x9, 0x28, 0x4, 0x4d, 0x40, 0x28, 0x1f, 0x52, 0x1, 0x0, 0xf, 0xe7, 0xd2, 0xf0, 0x7, 0x52, 0x1, 0x1, 0x7c, 0x2, 0x2e, 0x0, 0xb9, 0x63, 0x45, 0xca, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

using namespace std;

TsParserBase::TsParserBase()
{
	source = ES_SOURCE_DVB;
	index = 0;
	type = ES_VIDEO;

	mutex = PTHREAD_MUTEX_INITIALIZER;;
	packet = NULL;
	program = NULL;
	pid_list = NULL;
	last_cc_list = NULL;
	cc_error_number_list = NULL;
}

void TsParserBase::initialize(es_source_types _source, int _index, es_types _type, int pid_list_max_len)
{
	source = _source;
	index = _index;
	type = _type;

	mutex = PTHREAD_MUTEX_INITIALIZER;;
	packet = NULL;
	program = NULL;

	pid_list_length = 0;
	pid_list = new unsigned short int[pid_list_max_len];
	last_cc_list = new unsigned char[pid_list_max_len];
	cc_error_number_list = new unsigned short int[pid_list_max_len];
}

void TsParserBase::lock()
{
	pthread_mutex_lock(&mutex);
}

void TsParserBase::release()
{
	pthread_mutex_unlock(&mutex);
}

void TsParserBase::run(){}

int TsParserBase::getPidIndex()
{
	for(unsigned int i=0; i<pid_list_length; i++)
	{
		if(pid_list[i] == packet->header->pid)
			return i;
	}
	return -1;
}

void TsParserBase::add2PidList()
{
	pid_list[pid_list_length] = packet->header->pid;
	last_cc_list[pid_list_length] = packet->header->continuity_counter;
	cc_error_number_list[pid_list_length] = 0;
	pid_list_length++;
}

void TsParserBase::checkCC()
{
	int pid_index = getPidIndex();
	
	if(pid_index < 0)
	{
		add2PidList();
		return;
	}

	unsigned short int pid = packet->header->pid;
	unsigned char expected = (last_cc_list[pid_index] + 1) % 0x10;
	unsigned char received = packet->header->continuity_counter;
	
	if(expected != received)
	{
		writeDebugMessage(to_str(pid) + "_cc_errors.txt", now_str() + " expected: " + to_str((int)expected) + " received: " + to_str((int)received), ERROR);
		cc_error_number_list[pid_index]++;
		if(cc_error_number_list[pid_index] == CC_THRESHOLD_ERROR_NUMBER)
		{
			writeDebugMessage(to_str(pid) + "_cc_errors.txt", now_str() + "maximum number of cc error has been reached", ERROR);
		}
	}
	else
	{
		cc_error_number_list[pid_index] = 0;
	}
	last_cc_list[pid_index] = packet->header->continuity_counter;
}

unsigned int TsParserBase::getFirstPTS()
{
	return program->getFirstPTS();
}

void TsParserBase::syncByPTS(unsigned int PTS)
{
	program->syncByPTS(PTS);
}

void TsParserBase::prepareFrame()
{
	program->prepareFrame();
}

void TsParserBase::sendFrame()
{
	program->sendFrame();
}

void TsParserBase::printBufferStatus()
{
	program->printBufferStatus();
}

void TsParserBase::parse(unsigned char bytes[188])
{
	//lock();
	packet = new TsPacket(bytes);
	if(packet->packet_type == TS_UNKNOWN_PACKET)
		setPacketType();
	packet->check();
	packet->parse();
	if(packet->isParseable())
	{
		checkCC();
		if(write_input_to_file_flag)
		{
			writeRawData(to_str(packet->header->pid) + ".ts", packet->bytes, TS_PACKET_SIZE);
		}
		useParsedPacket();
	}
	delete packet;
	packet = NULL;
	//release();
}

bool TsParserBase::isPcrPacket()
{
	return program->isPcrPacket(packet->header->pid) && packet->hasPCR();
}

TsParserBase::~TsParserBase()
{
	delete []pid_list;
	delete []last_cc_list;
	delete []cc_error_number_list;
}

TsParserDVB::TsParserDVB()
{
	initialize(ES_SOURCE_DVB, 0, ES_ALL, 100);
#ifndef MOBILE_PLATFORM
	xml_output = NULL;
	xml_doc = NULL;
#endif
	pat_packet = NULL;
	sdt_packet = NULL;
	eit_packet = NULL;
	bat_packet = NULL;
	nit_packet = NULL;
	nit_pid = TS_NIT_PID;
	
	video_pcr_sender = NULL;
	audio_pcr_sender = NULL;
	audio_scd_pcr_sender = NULL;
	private_data_pcr_sender = NULL;

	program = new TsProgramDVB();

	if(send_pcr_flag && !ts_parser_mode)
	{
		video_pcr_sender = createClient(video_renderer_protocol, video_renderer_ip, video_renderer_port, true);
		audio_pcr_sender = createClient(audio_renderer_protocol, audio_renderer_ip, audio_renderer_port, true);
		audio_scd_pcr_sender = createClient(audio_secondary_renderer_protocol, audio_secondary_renderer_ip, audio_secondary_renderer_port, true);
		private_data_pcr_sender = createClient(private_data_renderer_protocol, private_data_renderer_ip, private_data_renderer_port, true);
	}
}

void TsParserDVB::run()
{
	processAdditionalContentUrl();
}

void TsParserDVB::setPacketType()
{
	if(pat_packet != NULL && pat_packet->packet_payload->nit_pid == packet->header->pid)
		packet->setPacketType(TS_NIT_PACKET);
	else if(isPmtPacket())
		packet->setPacketType(TS_PMT_PACKET);
	else if(isPesVideoPacket())
		packet->setPacketType(TS_PES_VIDEO_PACKET);
	else if(isPesAudioPacket())
		packet->setPacketType(TS_PES_AUDIO_PACKET);
	else if(isPesPrivateDataPacket())
		packet->setPacketType(TS_PES_PRIVATE_DATA_PACKET);
	/*else if(isPcrPacket())
		packet->setPacketType(TS_PCR_PACKET);*/
}

void TsParserDVB::createPatPacket()
{
	pat_packet = new TsPacket();
	*pat_packet = *packet;
	nit_pid = pat_packet->packet_payload->nit_pid;
	
	if(ts_parser_mode)
	{
		unsigned int number_of_programs = pat_packet->packet_payload->program_list_length - 1;
		printDebugMessage("number of programs in the stream: " + to_str(number_of_programs), WARNING);
		for(unsigned int i=0; i < pat_packet->packet_payload->program_list_length; i++)
		{
			if(pat_packet->packet_payload->program_list[i].program_no != TS_NIT_PROGRAM_NO)
			{
				printDebugMessage(to_str(i) + ") " + "program no: " + to_str(pat_packet->packet_payload->program_list[i].program_no) + "\t" + " pmt pid: "  + to_str(pat_packet->packet_payload->program_list[i].pmt_pid), WARNING);
			}
		}
		int user_choice;
		if(selected_program == 0)
		{
			user_choice = getUserProgramNumberChoice(number_of_programs);
		}
		else
		{
			user_choice = selected_program;
		}
		if(user_choice > 0)
		{
			program->setPmtPid(pat_packet->packet_payload->program_list[user_choice].pmt_pid);
			return;
		}
	}
	
	for(unsigned int i=0; i < pat_packet->packet_payload->program_list_length; i++)
	{
		if(pat_packet->packet_payload->program_list[i].program_no != TS_NIT_PROGRAM_NO)
		{
			program->setPmtPid(pat_packet->packet_payload->program_list[i].pmt_pid);
			break;
		}
	}
}

int TsParserDVB::getUserProgramNumberChoice(unsigned int number_of_programs)
{
	string input_str;
	cout << "Enter your program number choice: ";
	cin >> input_str;
	int input_int = atoi(input_str.c_str());
	
	return ((input_int < (int)number_of_programs) && (input_int > 0)) ? input_int : -1;
}

bool TsParserDVB::isPmtPacket()
{
	return program->isPmtPacket(packet->header->pid);
}

bool TsParserDVB::isPesVideoPacket()
{
	return program->isPesVideoPacket(packet->header->pid);
}

bool TsParserDVB::isPesAudioPacket()
{
	return program->isPesAudioPacket(packet->header->pid);
}

bool TsParserDVB::isPesPrivateDataPacket()
{
	return program->isPesPrivateDataPacket(packet->header->pid);
}

void TsParserDVB::processAdditionalContentUrl()
{
#ifndef MOBILE_PLATFORM
	if(additional_content_url_flag)
	{
		printDebugMessage("additional content url: " + sdt_packet->packet_payload->getUrl(), STATUS);
		xml_output = new CurlOutput();
		xml_output->processUrl(sdt_packet->packet_payload->getUrl());
	}
#endif
}

void TsParserDVB::useParsedPacket()
{
	if(isPcrPacket() && send_pcr_flag && !ts_parser_mode)
	{
		string pcr_json_message = "{\"id\":3401,\"jsonrpc\":\"2.0\",\"method\":\"AACRenderPCR\",\"params\":{\"ConnNumber\":34,\"PCR\":[\"" + to_string((unsigned long long int)packet->getPCRbase()) + "\",\"" + to_string((unsigned long long int)packet->getPCRextension()) + "\"]}}";
		//string pcr_json_message_audio = "{\"id\":3401,\"jsonrpc\":\"2.0\",\"method\":\"AACRenderPCR\",\"params\":{\"ConnNumber\":34,\"PCR\":[\"" + to_string((long long int)packet->getPCRbase()) + "\",\"" + to_string((long long int)packet->getPCRextension()) + "\"]}}";
		//printDebugMessage("sending PCR message: " + pcr_json_message, LOOP);
		video_pcr_sender->send(pcr_json_message.c_str(), strlen(pcr_json_message.c_str()));
		audio_pcr_sender->send(pcr_json_message.c_str(), strlen(pcr_json_message.c_str()));
		audio_scd_pcr_sender->send(pcr_json_message.c_str(), strlen(pcr_json_message.c_str()));
		private_data_pcr_sender->send(pcr_json_message.c_str(), strlen(pcr_json_message.c_str()));

	}
	
	switch(packet->packet_type)
	{
		case TS_PAT_PACKET:
			if(pat_packet == NULL)
			{
				createPatPacket();
				if(output_for_vlc_flag)
				{
					program->enhanced_video_frame_buffer->sender->send(pat_packet->bytes, TS_PACKET_SIZE);
					program->enhanced_audio_frame_buffer->sender->send(pat_packet->bytes, TS_PACKET_SIZE);
				}
			}
			if(ts_parser_mode && print_pat_details)
				packet->print();
			break;
		case TS_CAT_PACKET:
			if(ts_parser_mode && print_cat_details)
				packet->print();
			break;
		case TS_TSDT_PACKET:
			if(ts_parser_mode && print_tsdt_details)
				packet->print();
			break;
		case TS_NIT_PACKET:
			if(nit_packet == NULL)
			{
				nit_packet = new TsPacket();
			}
			*nit_packet = *packet;
			if(ts_parser_mode && print_nit_details)
				packet->print();
			break;
		case TS_SDT_PACKET:
			if(sdt_packet == NULL)
			{
				sdt_packet = new TsPacket();
				execute();
			}
			*sdt_packet = *packet;
			if(ts_parser_mode && print_sdt_details)
				packet->print();
			break;
		case TS_EIT_PACKET:
            if(eit_packet == NULL)
            {
                eit_packet = new TsPacket();
			}
			*eit_packet = *packet;
			if(ts_parser_mode && print_eit_details)
				packet->print();
            break;
        case TS_BAT_PACKET:
            if(bat_packet == NULL)
            {
                bat_packet = new TsPacket();
			}
            *bat_packet = *packet;
            if(ts_parser_mode && print_bat_details)
				packet->print();
            break;
		case TS_PMT_PACKET:
			program->setPmtPacket(packet);
			if(ts_parser_mode && print_pmt_details)
				packet->print();
			break;
		case TS_PES_VIDEO_PACKET:
			program->addPesPacket(packet);
			if(ts_parser_mode && print_pes_video_details)
				packet->print();
			break;
		case TS_PES_AUDIO_PACKET:
			program->addPesPacket(packet);
			if(ts_parser_mode && print_pes_audio_details)
				packet->print();
			break;
		case TS_PES_PRIVATE_DATA_PACKET:
			program->addPesPacket(packet);
			if(ts_parser_mode && print_pes_private_data_details)
				packet->print();
			break;
		case TS_PCR_PACKET:
			if(ts_parser_mode && print_pcr_details)
				packet->print();
		case TS_NULL_PACKET:
			if(ts_parser_mode && print_null_details)
				packet->print();
		case TS_UNKNOWN_PACKET:
			if(ts_parser_mode && print_unknown_details)
				packet->print();
		default:
			break;
	}
}

TsParserDVB::~TsParserDVB()
{
#ifndef MOBILE_PLATFORM
	delete xml_output;
	delete xml_doc;
#endif
	delete pat_packet;
	delete sdt_packet;
	delete eit_packet;
	delete bat_packet;
	delete nit_packet;
	delete video_pcr_sender;
	delete audio_pcr_sender;
	delete audio_scd_pcr_sender;
	delete private_data_pcr_sender;
	delete program;
}

TsParserP2PBase::TsParserP2PBase(){}

void TsParserP2PBase::initTs(int _index)
{
	initialize(ES_SOURCE_P2P, _index, ES_VIDEO, 10);
}

void TsParserP2PBase::setPacketType()
{
	if(program->isPesVideoPacket(packet->header->pid))
		packet->setPacketType(TS_PES_VIDEO_PACKET);
}

void TsParserP2PBase::useParsedPacket()
{
	program->addPesPacket(packet);
}

TsParserP2PBase::~TsParserP2PBase(){}

TsParserP2PVideoBase::TsParserP2PVideoBase(){}

TsParserP2PVideoBase::TsParserP2PVideoBase(int _index, unsigned short int _es_pid)
{
	initTs(_index);
	program = new TsProgramP2PVideo(index, _es_pid);
	
	video_pcr_sender = NULL;
	audio_pcr_sender = NULL;
	audio_scd_pcr_sender = NULL;
	private_data_pcr_sender = NULL;

	if(no_dvb_flag && (index == 0) && send_pcr_flag && !ts_parser_mode)
	{
		video_pcr_sender = createClient(video_renderer_protocol, video_renderer_ip, video_renderer_port, true);
		audio_pcr_sender = createClient(audio_renderer_protocol, audio_renderer_ip, audio_renderer_port, true);
		audio_scd_pcr_sender = createClient(audio_secondary_renderer_protocol, audio_secondary_renderer_ip, audio_secondary_renderer_port, true);
		private_data_pcr_sender = createClient(private_data_renderer_protocol, private_data_renderer_ip, private_data_renderer_port, true);
	}


	if(output_for_vlc_flag)
	{
		program->enhanced_video_frame_buffer->sender->send(PAT_PCKT, TS_PACKET_SIZE);
		program->enhanced_video_frame_buffer->sender->send(PMT_PCKT, TS_PACKET_SIZE);
	}
}

void TsParserP2PVideoBase::useParsedPacket()
{
	if(no_dvb_flag && (index == 0) && isPcrPacket() && send_pcr_flag && !ts_parser_mode)
	{
		string pcr_json_message = "{\"id\":3401,\"jsonrpc\":\"2.0\",\"method\":\"AACRenderPCR\",\"params\":{\"ConnNumber\":34,\"PCR\":[\"" + to_string((unsigned long long int)packet->getPCRbase()) + "\",\"" + to_string((unsigned long long int)packet->getPCRextension()) + "\"]}}";
		//string pcr_json_message_audio = "{\"id\":3401,\"jsonrpc\":\"2.0\",\"method\":\"AACRenderPCR\",\"params\":{\"ConnNumber\":34,\"PCR\":[\"" + to_string((long long int)packet->getPCRbase()) + "\",\"" + to_string((long long int)packet->getPCRextension()) + "\"]}}";
		//printDebugMessage("sending PCR message: " + pcr_json_message, LOOP);
		video_pcr_sender->send(pcr_json_message.c_str(), strlen(pcr_json_message.c_str()));
		audio_pcr_sender->send(pcr_json_message.c_str(), strlen(pcr_json_message.c_str()));
		audio_scd_pcr_sender->send(pcr_json_message.c_str(), strlen(pcr_json_message.c_str()));
		private_data_pcr_sender->send(pcr_json_message.c_str(), strlen(pcr_json_message.c_str()));

	}
	program->addPesPacket(packet);
}

TsParserP2PVideoBase::~TsParserP2PVideoBase()
{
	delete video_pcr_sender;
	delete audio_pcr_sender;
	delete audio_scd_pcr_sender;
	delete private_data_pcr_sender;
	delete program;
}

TsParserP2PVideoEnhancement::TsParserP2PVideoEnhancement(TsParserBase *parser)
{
	initTs(parser->index);
	program = parser->program;
}
TsParserP2PVideoEnhancement::~TsParserP2PVideoEnhancement(){}

TsParserP2PAudio::TsParserP2PAudio(int _index, unsigned short int _es_pid)
{
	initialize(ES_SOURCE_P2P, _index, ES_AUDIO, 10);
	program = new TsProgramP2PAudio(index, _es_pid);
}

void TsParserP2PAudio::setPacketType()
{
	if(program->isPesAudioPacket(packet->header->pid))
		packet->setPacketType(TS_PES_AUDIO_PACKET);
}

void TsParserP2PAudio::useParsedPacket()
{
	program->addPesPacket(packet);
}

TsParserP2PAudio::~TsParserP2PAudio()
{
	delete program;
}

TsParserP2PPrivateData::TsParserP2PPrivateData(int _index, unsigned short int _es_pid)
{
	initialize(ES_SOURCE_P2P, _index, ES_PRIVATE_DATA, 10);
	program = new TsProgramP2PPrivateData(index, _es_pid);
}

void TsParserP2PPrivateData::setPacketType()
{
	if(program->isPesPrivateDataPacket(packet->header->pid))
		packet->setPacketType(TS_PES_PRIVATE_DATA_PACKET);
}

void TsParserP2PPrivateData::useParsedPacket()
{
	program->addPesPacket(packet);
}

TsParserP2PPrivateData::~TsParserP2PPrivateData()
{
	delete program;
}
