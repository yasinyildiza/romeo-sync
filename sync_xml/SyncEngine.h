#ifndef _SYNC_ENGINE_H
#define _SYNC_ENGINE_H

#include <cstdlib>
#include <stdlib.h>
#include <algorithm>
#include <limits>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <string>

#include "TCPserver.h"
#include "json.h"
#include "ConfigFileParser.h"
#include "Thread.h"
#include "TsReceiver.h"
#include "SocketClient.h"

class SyncEngine : public Thread
{
	private:

		/* dvb tune values */
		int frequency;		
		fe_bandwidth_t bandwidth;
		fe_code_rate_t code_rate_HP;
		fe_code_rate_t code_rate_LP;
		fe_spectral_inversion_t inversion;
		fe_modulation_t constellation;
		fe_transmit_mode_t transmission_mode;
		fe_guard_interval_t guard_interval;
		fe_hierarchy_t hierarchy_information;
		
		/* UI message receiver elements */
		TCPserver *server;
		char *ui_json_msg;
		json_value *ui_json_root;

		/*overlay client for collaboration */
		SocketClient *overlay_client;
		//bool overlay_connection_failure;

		TsReceiverBase **receivers;
		int total_number_of_receivers;
		int number_of_receivers;
		int total_number_of_base_receivers;
		int number_of_base_receivers;
		int number_of_tuned_receivers;
		
		unsigned long long pts2send;
		int loop_counter;

		void setParamsFromUI();
		bool setPTS();
		void syncByPTS();
		void printBufferStatuses();
		void start2send();
		void increasePTS();
		void sendPTS2OverlayClient();
		void receiveCollaborativePTS();
		bool emptyBufferExists();

	public:

		SyncEngine();
		void run();
		void waitForStopMessage();
		void stopAll();
		~SyncEngine();
};

#endif
