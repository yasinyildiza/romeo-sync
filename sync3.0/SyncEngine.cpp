#include <iostream>

#include "SyncEngine.h"
#include "debugMessagePrinter.h"

using namespace std;

SyncEngine::SyncEngine()
{
	system("make log_clean");
	server = NULL;
	overlay_client = NULL;
	pts2send = 0;
	loop_counter = 0;
	number_of_receivers = 0;
	number_of_base_receivers = 0;
	number_of_tuned_receivers = 0;
	//overlay_connection_failure = false;

	if(include_ui)
		setParamsFromUI();

	total_number_of_receivers = number_of_dvb_receivers + number_of_p2p_video_base_receivers + number_of_p2p_video_enhancement_receivers + number_of_p2p_audio_receivers + number_of_p2p_private_data_receivers + number_of_file_receivers;
	total_number_of_base_receivers = number_of_dvb_receivers + number_of_p2p_video_base_receivers + number_of_p2p_audio_receivers + number_of_p2p_private_data_receivers + number_of_file_receivers;

	printDebugMessage("dvb: " + to_str(number_of_dvb_receivers), INFO);
	printDebugMessage("p2p video base: " + to_str(number_of_p2p_video_base_receivers), INFO);
	printDebugMessage("p2p video enhancement: " + to_str(number_of_p2p_video_enhancement_receivers), INFO);
	printDebugMessage("p2p audio: " + to_str(number_of_p2p_audio_receivers), INFO);
	printDebugMessage("p2p private data: " + to_str(number_of_p2p_private_data_receivers), INFO);
	printDebugMessage("file: " + to_str(number_of_file_receivers), INFO);
	printDebugMessage("total: " + to_str(total_number_of_receivers), INFO);

	receivers = new TsReceiverBase*[total_number_of_receivers];

	for(int i=0; i<number_of_dvb_receivers; i++)
	{
		if(include_ui)
			receivers[number_of_receivers] = new TsReceiverDVB(frequency, bandwidth, code_rate_HP, code_rate_LP, inversion, constellation, transmission_mode, guard_interval, hierarchy_information);
		else
			receivers[number_of_receivers] = new TsReceiverDVB();
		if(receivers[number_of_receivers]->tune(NULL))
			number_of_tuned_receivers++;
		else
			printDebugMessage("receiver " + receivers[number_of_receivers]->receiver_id + " could not tune", WARNING);
		number_of_receivers++;
		number_of_base_receivers++;
	}

	for(int i=0; i<number_of_p2p_video_base_receivers; i++)
	{
		receivers[number_of_receivers] = new TsReceiverP2PVideoBase(i);
		if(receivers[number_of_receivers]->tune(NULL))
			number_of_tuned_receivers++;
		else
			printDebugMessage("receiver " + receivers[number_of_receivers]->receiver_id + " could not tune", WARNING);
		number_of_receivers++;
		number_of_base_receivers++;
	}

	for(int i=0; i<number_of_p2p_video_enhancement_receivers; i++)
	{
		receivers[number_of_receivers] = new TsReceiverP2PVideoEnhancement(i);
		if(receivers[number_of_receivers]->tune(receivers[number_of_dvb_receivers+i]->ts_parser))
			number_of_tuned_receivers++;
		else
			printDebugMessage("receiver " + receivers[number_of_receivers]->receiver_id + " could not tune", WARNING);
		number_of_receivers++;
	}

	for(int i=0; i<number_of_p2p_audio_receivers; i++)
	{
		receivers[number_of_receivers] = new TsReceiverP2PAudio(i);
		if(receivers[number_of_receivers]->tune(NULL))
			number_of_tuned_receivers++;
		else
			printDebugMessage("receiver " + receivers[number_of_receivers]->receiver_id + " could not tune", WARNING);
		number_of_receivers++;
		number_of_base_receivers++;
	}

	for(int i=0; i<number_of_p2p_private_data_receivers; i++)
	{
		receivers[number_of_receivers] = new TsReceiverP2PPrivateData(i);
		if(receivers[number_of_receivers]->tune(NULL))
			number_of_tuned_receivers++;
		else
			printDebugMessage("receiver " + receivers[number_of_receivers]->receiver_id + " could not tune", WARNING);
		number_of_receivers++;
		number_of_base_receivers++;
	}

	for(int i=0; i<number_of_file_receivers; i++)
	{
		receivers[number_of_receivers] = new TsReceiverTsFile(stream_file_name);
		if(receivers[number_of_receivers]->tune(NULL))
			number_of_tuned_receivers++;
		else
			printDebugMessage("receiver " + receivers[number_of_receivers]->receiver_id + " could not tune", WARNING);
		number_of_receivers++;
		number_of_base_receivers++;
	}

	if(number_of_receivers != total_number_of_receivers)
	{
		printDebugMessage("number of receivers mismatch: " + to_str(number_of_receivers) + "/" + to_str(total_number_of_receivers), ERROR);
	}

	if(number_of_base_receivers != total_number_of_base_receivers)
	{
		printDebugMessage("number of base receivers mismatch: " + to_str(number_of_base_receivers) + "/" + to_str(total_number_of_base_receivers), ERROR);
	}

	if(number_of_tuned_receivers != total_number_of_receivers)
	{
		printDebugMessage("number of tuned receivers mismatch: " + to_str(number_of_tuned_receivers) + "/" + to_str(total_number_of_receivers), ERROR);
	}
}

void SyncEngine::setParamsFromUI()
{
	printDebugMessage("creating TCP server...", INFO);
	server = new TCPserver(server_port_no);
	printDebugMessage("created TCP server", INFO);
	printDebugMessage("waiting for UI module to connect...", STATUS);
	server->acceptClient();
	printDebugMessage("UI module connected", STATUS);
	printDebugMessage("waiting for UI module to send tune params...", STATUS);
	ui_json_msg = new char[201];
	server->recv(ui_json_msg, 200);
	ui_json_msg[200] = 0x0;
	printDebugMessage("tune params received", STATUS);
	server->closeSocket();
	printDebugMessage("server closed", INFO);
	
	printDebugMessage("parsing received JSON message...", INFO);
	char *errorPos = 0;
	char *errorDesc = 0;
	int errorLine = 0;
	block_allocator allocator(1 << 10); // 1 KB per block
	ui_json_root = json_parse(ui_json_msg, &errorPos, &errorDesc, &errorLine, &allocator);
	if(ui_json_root == NULL)
	{
		printDebugMessage("JSON parse error", ERROR);
		printDebugMessage("Error at line: " +  to_str(errorLine), ERROR);
		return;
	}
	printDebugMessage("getting DVB parameters from JSON message...", INFO);
	parseSyncJSON(ui_json_root, &frequency, &bandwidth, &code_rate_HP, &code_rate_LP, &inversion, &constellation, &transmission_mode, &guard_interval, &hierarchy_information);
	printDebugMessage("frequency: " + to_str(frequency), INFO);
	printDebugMessage("bandwidth: " + to_str(bandwidth), INFO);
	printDebugMessage("code_rate_HP: " + to_str(code_rate_HP), INFO);
	printDebugMessage("code_rate_LP: " + to_str(code_rate_LP), INFO);
	printDebugMessage("inversion: " + to_str(inversion), INFO);
	printDebugMessage("constellation: " + to_str(constellation), INFO);
	printDebugMessage("transmission_mode: " + to_str(transmission_mode), INFO);
	printDebugMessage("guard_interval: " + to_str(guard_interval), INFO);
	printDebugMessage("hierarchy_information: " + to_str(hierarchy_information), INFO);
	printDebugMessage("received JSON message parsed", INFO);
}

bool SyncEngine::emptyBufferExists()
{
	unsigned int temp;
	for(int i=0; i<number_of_receivers; i++)
	{
		temp = receivers[i]->getFirstPTS();
		if(temp == 0)
			return true;
	}
	return false;
}

bool SyncEngine::setPTS()
{
	unsigned int temp;
	
	if(emptyBufferExists())
	{
		int counter = 0;
		while(true)
		{
			usleep(ONE_SECOND);
			counter++;
			if(!emptyBufferExists() || counter >= sync_timeout_as_seconds)
				break;
		}
	}
	
	for(int i=0; i<number_of_receivers; i++)
	{
		temp = receivers[i]->getFirstPTS();
		if(receivers[i]->type == ES_ALL || receivers[i]->type == ES_VIDEO)
			pts2send = max((unsigned int)pts2send, temp);
	}
	
	printDebugMessage("synchronization PTS: " + to_str(pts2send), INFO);
	return pts2send > 0;
}

void SyncEngine::syncByPTS()
{
	if(!do_not_sync)
	{
		for(int i=0; i<number_of_receivers; i++)
		{
			if(receivers[i]->subtype != ES_ENHANCEMENT)
				receivers[i]->syncByPTS(pts2send);
		}
	}
}

void SyncEngine::startAudioDecoder()
{
	SocketClient *audioStarter = createClient(audio_decoder_protocol, audio_decoder_ip_base, audio_start_port, true);
	string audio_starter_message = "{\"id\":6002,\"jsonrpc\":\"2.0\",\"method\":\"RemoteRunStart\",\"params\":{}}";
	audioStarter->send(audio_starter_message.c_str(), strlen(audio_starter_message.c_str()));
	cout << "start message (" << audio_starter_message << ") sent to audio decoder: " << audio_decoder_protocol << "|" << audio_decoder_ip_base << ":" << audio_start_port << endl;
	delete audioStarter;
		
	audioStarter = createClient(audio_secondary_decoder_protocol, audio_secondary_decoder_ip_base, audio_start_port, true);
	audioStarter->send(audio_starter_message.c_str(), strlen(audio_starter_message.c_str()));
	cout << "start message (" << audio_starter_message << ") sent to audio decoder: " << audio_secondary_decoder_protocol << "|" << audio_secondary_decoder_ip_base << ":" << audio_start_port << endl;
	delete audioStarter;
}

void SyncEngine::stopAudioDecoder()
{
	SocketClient *audioStoper = createClient(audio_decoder_protocol, audio_decoder_ip_base, audio_start_port, true);
	string audio_stoper_message = "{\"id\":3002,\"jsonrpc\":\"2.0\",\"method\":\"RemoteRunStop\",\"params\":{}}";
	audioStoper->send(audio_stoper_message.c_str(), strlen(audio_stoper_message.c_str()));
	cout << "stop message (" << audio_stoper_message << ") sent to audio decoder: " << audio_decoder_protocol << "|" << audio_decoder_ip_base << ":" << audio_start_port << endl;
	delete audioStoper;
	
	audioStoper = createClient(audio_secondary_decoder_protocol, audio_secondary_decoder_ip_base, audio_start_port, true);
	audioStoper->send(audio_stoper_message.c_str(), strlen(audio_stoper_message.c_str()));
	cout << "stop message (" << audio_stoper_message << ") sent to audio decoder: " << audio_secondary_decoder_protocol << "|" << audio_secondary_decoder_ip_base << ":" << audio_start_port << endl;
	delete audioStoper;
}

void SyncEngine::start2send()
{
	printDebugMessage("prebuffering...", STATUS);
	usleep((prebuffering_as_seconds * ONE_SECOND) / 2);

	startAudioDecoder();

	if(!do_not_consume)
	{
		for(int i=0; i<number_of_receivers; i++)
		{
			if(receivers[i]->subtype != ES_ENHANCEMENT)
				receivers[i]->start2prepareTsEnhancedFrames();
		}
		
		for(int i=0; i<number_of_receivers; i++)
		{
			if(receivers[i]->subtype != ES_ENHANCEMENT)
				receivers[i]->start2sendTsEnhancedFrames();
		}
	}
}

void SyncEngine::printBufferStatuses()
{
	if(print_fill_ratio)
	{
		for(int i=0; i<number_of_receivers; i++)
		{
			receivers[i]->printBufferStatus();
		}
	}
}

void SyncEngine::increasePTS()
{
	for(int i=0; i<number_of_receivers; i++)
	{
		if(receivers[i]->subtype != ES_ENHANCEMENT)
		{
			pthread_mutex_lock(&(receivers[i]->count_lock));
			if((receivers[i]->count) == 0)
				pthread_cond_signal(&(receivers[i]->count_nonzero));
			(receivers[i]->count) = (receivers[i]->count) + 1;
		}
	}
    //writeDebugMessage("sync_pts.txt", to_str(pts2send) + "\t" + now_str(), INFO);
	pts2send += PTS_PER_FRAME_VIDEO;
    for(int i=0; i<number_of_receivers; i++)
    {
		if(receivers[i]->subtype != ES_ENHANCEMENT)
		{
			pthread_mutex_unlock(&(receivers[i]->count_lock));
		}
	}
	
	
	if(loop_counter * USECOND_PER_FRAME < (prebuffering_as_seconds * ONE_SECOND) / 2)
	{
		if(loop_counter == 0)
		{
			printDebugMessage("prebuffering...", STATUS);
		}
		loop_counter++;
	}
	else
	{
		for(int i=0; i<number_of_receivers; i++)
		{
			if(receivers[i]->subtype != ES_ENHANCEMENT)
			{
				pthread_mutex_lock(&(receivers[i]->count_lock2));
				if((receivers[i]->count2) == 0)
					pthread_cond_signal(&(receivers[i]->count_nonzero2));
				(receivers[i]->count2) = (receivers[i]->count2) + 1;
			}
		}
		//writeDebugMessage("sync_pts.txt", to_str(pts2send) + "\t" + now_str(), INFO);
		pts2send += PTS_PER_FRAME_VIDEO;
		for(int i=0; i<number_of_receivers; i++)
		{
			if(receivers[i]->subtype != ES_ENHANCEMENT)
			{
				pthread_mutex_unlock(&(receivers[i]->count_lock2));
			}
		}
	}
}

void SyncEngine::sendPTS2OverlayClient()
{
	
	printDebugMessage("connecting to overlay client...", STATUS);
	overlay_client = createClient(overlay_client_protocol, overlay_client_ip, overlay_client_port, true);
	if(!(overlay_client->is_connected))
	{
		printDebugMessage("could not connect to overlay client", STATUS);
		return;
	}
	printDebugMessage("connected to overlay client", STATUS);
/*
	unsigned char data[8];
	unsigned char PTS_str[8];
	memcpy(PTS_str, &pts2send, 4);
	memset(data, 0, 4);
	memcpy(data+4, PTS_str+3, 1);
	memcpy(data+5, PTS_str+2, 1);
	memcpy(data+6, PTS_str+1, 1);
	memcpy(data+7, PTS_str, 1);
*/	
	printDebugMessage("sending max PTS to overlay client...", STATUS);
	string json_message = "{\"id\":3406, \"jsonrpc\":\"2.0\", \"method\":\"setClientPTS\", \"params\":{\"PTS\":\"" + to_str(pts2send) + "\", \"BufferSize\":\"" + to_str(buffer_size_as_seconds) + "\"}}";
	cout << json_message << endl;
	int json_message_length  = strlen(json_message.c_str());
	string json_message_length_str = to_str(json_message_length);
	int json_message_length_str_length = strlen(json_message_length_str.c_str());
	if(json_message_length < 100)
	{
		json_message_length_str = "0" + json_message_length_str;
		json_message_length_str_length++;
	}
	//overlay_client->send(json_message_length_str.c_str(), json_message_length_str_length);
	overlay_client->send(json_message.c_str(), json_message_length);
	printDebugMessage("sent max PTS to overlay client at " + now_str(), STATUS);
}

void SyncEngine::receiveCollaborativePTS()
{
	if(!(overlay_client->is_connected))
		return;
	printDebugMessage("receiving collaborative PTS from overlay client...", STATUS);
	char *data = new char[8];
	overlay_client->recv(data, 8);
	printDebugMessage("received collaborative PTS from overlay client at " + now_str(), STATUS);
	/*cout << "received data: ";
	for(int i=0; i<8; i++)
	{
		cout << (int)data[i] << " ";
	}
	cout << endl;*/
	char temp[8];
	memcpy(temp+7, data+0, 1);
	memcpy(temp+6, data+1, 1);
	memcpy(temp+5, data+2, 1);
	memcpy(temp+4, data+3, 1);
	memcpy(temp+3, data+4, 1);
	memcpy(temp+2, data+5, 1);
	memcpy(temp+1, data+6, 1);
	memcpy(temp+0, data+7, 1);
	memcpy((&pts2send), temp, 8);
	printDebugMessage("collaborative PTS: " + to_str(pts2send), STATUS);
	if(pts2send == 0)
	{
		printDebugMessage("PTS cannot be zero!", ERROR);
		exit();
	}
	delete []data;
	//overlay_client->closeSocket();
}

void SyncEngine::run()
{
	if(ts_parser_mode)
	{
		while(true)
		{
			usleep(ONE_SECOND);
		}
	}
	//usleep(sync_timeout_as_seconds * ONE_SECOND);
	if(setPTS())
	{
		if(collaborative_sync_flag)
		{
			sendPTS2OverlayClient();
			receiveCollaborativePTS();
		}
		printDebugMessage("pts2send: " + to_str(pts2send), INFO);
		syncByPTS();
		usleep(ONE_SECOND);
		if(collaborative_sync_flag && !(overlay_client->is_connected))
		{
			string json_message = "{\"id\":3406, \"jsonrpc\":\"2.0\", \"method\":\"setReady\", \"params\":{}}";
			int json_message_length  = strlen(json_message.c_str());
			overlay_client->send(json_message.c_str(), json_message_length);
			char start_message[8];
			overlay_client->recv(start_message, 8);
		}
		start2send();

		long long int t;
		long long int d;
		while(true)
		{
			t = getMicroSeconds();
			printBufferStatuses();
			increasePTS();
			pthread_testcancel();
			d = USECOND_PER_FRAME - (getMicroSeconds() - t) - constant_delay;
			usleep((d > 0 ? d : 0));
			//usleep(USECOND_PER_FRAME - (getMicroSeconds() - t) - constant_delay);
		}
	}
	else
	{
		printDebugMessage("receivers received nothing", ERROR);
	}
}

void SyncEngine::stopAll()
{
	for(int i=0; i<number_of_receivers; i++)
	{
		receivers[i]->stopThreads();
		usleep(1);
	}
}

void SyncEngine::waitForStopMessage()
{
	while(true)
	{
		if(include_ui)
		{
			delete ui_json_msg;
			ui_json_msg = new char[4];
			server->recv(ui_json_msg, 4);
			delete ui_json_msg;
			break;
		}
		else
		{
			string user_exit;
			cin >> user_exit;
			if(user_exit == "exit")
				break;
		}
	}
}

SyncEngine::~SyncEngine()
{
	for(int i=0; i<number_of_receivers; i++)
		delete receivers[i];
	delete []receivers;
	if(include_ui)
		delete []ui_json_msg;
	delete server;
	delete overlay_client;
}
