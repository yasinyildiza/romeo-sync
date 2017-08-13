#ifndef _TCP_CLIENT_H
#define _TCP_CLIENT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "SocketClient.h"

class TCPclient : public SocketClient
{
	private:
	
		int sockfd;
		struct sockaddr_in serv_addr;
		struct hostent *server;
	
	public:

		std::string hostname;
		int portno;
	
		TCPclient(const std::string host_name, const int port_no);
		virtual bool connect2Server();
		unsigned char *recv();
		void recv(char *msg, const int len);
		virtual void send(const unsigned char *msg, const int len);
		virtual void send(const char *msg, const int len);
		void closeSocket();
		void error(const char *msg, const bool high);
		virtual ~TCPclient();
};

#endif
