#include <iostream>
#include <csignal>
#include <cstdlib>
#include <cstdio>

#include "debugMessagePrinter.h"
#include "ConfigFileParser.h"
#include "TsPacketBuffer.h"
#include "TsFrameBuffer.h"
#include "TsEnhancedFrameBuffer.h"
#include "TsEs.h"
#include "SocketClient.h"
#include "TCPserver.h"
#include "UDPserver.h"
#include "globals.h"
#include "TsProgram.h"
#include "TsParser.h"
#include "Thread.h"
#include "DvbObjects.h"
#include "DvbSubdevices.h"
#include "TsReceiver.h"
#include "SyncEngine.h"

using namespace std;

SyncEngine *syncer;

void handleCtrlC(int s)
{
	printf("Caught signal %d\n",s);
	syncer->stopAudioDecoder();
	exit(1);
}

int main(int argc, char* argv[])
{
	signal(SIGINT, handleCtrlC);
	configureAll();
	setDebugLevel(argc, argv);
	printDebugMessage("START OF PROGRAM", WARNING);
	syncer = new SyncEngine();
	usleep(ONE_SECOND);
	syncer->execute();
	//syncer->run();
	usleep(ONE_SECOND);
	//syncer->waitForStopMessage();
	syncer->join();
	usleep(ONE_SECOND);
	syncer->stopAll();
	usleep(ONE_SECOND);
	syncer->stop();
	usleep(ONE_SECOND);
	delete syncer;
	printDebugMessage("END OF PROGRAM", WARNING);
	return 0;
}
