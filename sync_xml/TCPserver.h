#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

class TCPserver
{
	private:
	
		int sockfd, newsockfd, portno;
		socklen_t clilen;
		struct sockaddr_in serv_addr, cli_addr;
	
	public:
	
		TCPserver(const int port_no);
		void acceptClient();
		bool recv(char *msg, int len);
		void send(const char *msg);
		void closeSocket();
		void error(const char *msg, const bool high);
		~TCPserver();
};

#endif