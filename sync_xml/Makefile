CC=g++
CFLAGS=-c -Wall -g -O0 -std=c++0x
LDFLAGS=-L/usr/lib/i386-linux-gnu -pthread
ifneq ($(P), m)
	LDFLAGS+=-lcurl
	SOURCES+= tinystr.cpp
	SOURCES+= tinyxml.cpp
	SOURCES+= tinyxmlerror.cpp
	SOURCES+= tinyxmlparser.cpp
	SOURCES+= CurlOutput.cpp
endif
SOURCES+= SyncXmlParser.cpp
SOURCES+= XmlObjects.cpp
SOURCES+= block_allocator.cpp
SOURCES+= json.cpp
SOURCES+= debugMessagePrinter.cpp
SOURCES+= Thread.cpp
SOURCES+= ConfigFileParser.cpp
SOURCES+= globals.cpp
SOURCES+= SocketClient.cpp
SOURCES+= TCPserver.cpp
SOURCES+= UDPserver.cpp
SOURCES+= TsPacketContainer.cpp
SOURCES+= TsPacketBuffer.cpp
SOURCES+= TsPacket.cpp
SOURCES+= TsPes.cpp
SOURCES+= FrameBase.cpp
SOURCES+= TsFrame.cpp
SOURCES+= TsFrameBuffer.cpp
SOURCES+= TsEs.cpp
SOURCES+= TsEnhancedFrame.cpp
SOURCES+= TsEnhancedFrameBuffer.cpp
SOURCES+= TsProgram.cpp
SOURCES+= TsParser.cpp
SOURCES+= DvbObjects.cpp
SOURCES+= DvbSubdevices.cpp
SOURCES+= TsReceiver.cpp
SOURCES+= SyncEngine.cpp
SOURCES+= main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sync

#PLATFORMS:
	#m: mobile
	#p: portable
	#f: fixed

ifeq ($(P), m)
	OPTS = -DMOBILE_PLATFORM
endif

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(OPTS) $(CFLAGS) $< -o $@

log_clean:
	rm -rf log/*

clean:
	rm sync *.o

release:
	rm sync *.o -rf log/*
