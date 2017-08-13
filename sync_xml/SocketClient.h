#ifndef _SOCKET_CLIENT_H
#define _SOCKET_CLIENT_H

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
#include <errno.h>

#define MAXIMUM_DATAGRAM_SIZE 50000

class SocketClient
{
	private:
	protected:
		std::string hostname;
		int portno;
		int sockfd;
		struct sockaddr_in serveraddr;
		struct hostent *server;
		unsigned int serverlen;
		
		void error(const char *msg, const bool high);
	public:
		bool is_connected;
		SocketClient();
		virtual void send(const unsigned char *msg, const int len) = 0;
		virtual void send(const char *msg, const int len) = 0;
		virtual unsigned char *recv(const int len) = 0;
		virtual void recv(char *msg, const int len) = 0;
		virtual void connect2Server() = 0;
		virtual ~SocketClient();
};

class UDPclient : public SocketClient
{
	private:
	
	public:

		UDPclient(const std::string host_name, const int port_no);
		virtual void connect2Server();
		unsigned char *recv(const int len);
		void recv(char *msg, const int len);
		virtual void send(const unsigned char *msg, const int len);
		virtual void send(const char *msg, const int len);
		virtual ~UDPclient();
};

class TCPclient : public SocketClient
{
	private:
	
	public:

		TCPclient(const std::string host_name, const int port_no);
		virtual void connect2Server();
		unsigned char *recv(const int len);
		void recv(char *msg, const int len);
		virtual void send(const unsigned char *msg, const int len);
		virtual void send(const char *msg, const int len);
		void closeSocket();
		virtual ~TCPclient();
};

SocketClient *createClient(const std::string protocol, const std::string host_name, const int port_no, const bool immediate_connect);

#endif
