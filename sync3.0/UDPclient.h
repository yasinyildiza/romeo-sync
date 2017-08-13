#ifndef _UDP_CLIENT_H
#define _UDP_CLIENT_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "SocketClient.h"

#define MAXIMUM_DATAGRAM_SIZE 50000

class UDPclient : public SocketClient
{
	private:
	
		int sockfd;
		unsigned int serverlen;
		struct sockaddr_in serveraddr;
		struct hostent *server;
		std::string hostname;
		int portno;
	
	public:

		UDPclient(const std::string host_name, int port_no);
		virtual bool connect2Server();
		unsigned char *recv(const int len);
		virtual void send(const unsigned char *msg, const int len);
		virtual void send(const char *msg, const int len);
		void error(const char *msg, const bool high);
		virtual ~UDPclient();
};
 
#endif
