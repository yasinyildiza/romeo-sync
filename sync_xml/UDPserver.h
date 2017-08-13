#ifndef _UDP_SERVER_H
#define _UDP_SERVER_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

class UDPserver
{
	private:
	
		int sockfd; /* socket */
		int portno; /* port to listen on */
		unsigned int clientlen; /* byte size of client's address */
		struct sockaddr_in serveraddr; /* server's addr */
		struct sockaddr_in clientaddr; /* client addr */
		int optval; /* flag value for setsockopt */
		int n;
	
	public:
	
		UDPserver(int port_no);
		void recv(unsigned char *msg, const int len);
		int getLastMessageLength();
		void send(const unsigned char *msg, const int len);
		void error(const char *msg, const bool high);
		~UDPserver();
};

#endif