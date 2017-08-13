#include <iostream>

#include "SocketClient.h"

using namespace std;

SocketClient::SocketClient()
{
	is_connected = false;
}

void SocketClient::error(const char *msg, const bool high)
{
    cout << hostname << ": " << portno << " " << msg << " " << errno << endl;
}

SocketClient::~SocketClient(){}

UDPclient::UDPclient(const std::string host_name, int port_no)
{
	hostname = host_name;
	portno = port_no;
	
	/* socket: create the socket */
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) error("ERROR opening socket", true);
	
	/* gethostbyname: get the server's DNS entry */
	server = gethostbyname(hostname.c_str());
	if (server == NULL)	error("ERROR, no such host", true);
	
	/* build the server's Internet address */
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
	serveraddr.sin_port = htons(portno);
	serverlen = sizeof(serveraddr);
}

void UDPclient::connect2Server()
{
	is_connected = true;
}

unsigned char *UDPclient::recv(const int len)
{
	unsigned char *msg = new unsigned char[len];
	int n = recvfrom(sockfd, (void *)msg, len, 0, (sockaddr*)&serveraddr, &serverlen);
	if(n < 0) error("ERROR in recv", false);
	return msg;
}

void UDPclient::recv(char *msg, int len)
{
	int n = recvfrom(sockfd, (void *)msg, len, 0, (sockaddr*)&serveraddr, &serverlen);
	if(n < 0) error("ERROR in recv", false);
}

void UDPclient::send(const unsigned char *msg, const int len)
{
	int number_of_send = len / MAXIMUM_DATAGRAM_SIZE;
	int offset = 0;
	for(int i=0; i<number_of_send; i++)
	{
		if(sendto(sockfd, msg+offset, MAXIMUM_DATAGRAM_SIZE, 0, (sockaddr*)&serveraddr, serverlen) < 0) error("ERROR in sendto", false);
		offset += MAXIMUM_DATAGRAM_SIZE;
	}

	if(sendto(sockfd, msg+offset, len-number_of_send*MAXIMUM_DATAGRAM_SIZE, 0, (sockaddr*)&serveraddr, serverlen) < 0) error("ERROR in sendto", false);
}

void UDPclient::send(const char *msg, const int len)
{
	int number_of_send = len / MAXIMUM_DATAGRAM_SIZE;
	int offset = 0;
	for(int i=0; i<number_of_send; i++)
	{
		if(sendto(sockfd, msg+offset, MAXIMUM_DATAGRAM_SIZE, 0, (sockaddr*)&serveraddr, serverlen) < 0) error("ERROR in sendto", false);
		offset += MAXIMUM_DATAGRAM_SIZE;
	}

	if(sendto(sockfd, msg+offset, len-number_of_send*MAXIMUM_DATAGRAM_SIZE, 0, (sockaddr*)&serveraddr, serverlen) < 0) error("ERROR in sendto", false);
}

UDPclient::~UDPclient(){}

TCPclient::TCPclient(const std::string host_name, const int port_no)
{
	hostname = host_name;
	portno = port_no;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket", true);
}

void TCPclient::connect2Server()
{
	server = gethostbyname(hostname.c_str());
    if (server == NULL) error("ERROR, no such host", true);

	serverlen = sizeof(serveraddr);
    bzero((char *) &serveraddr, serverlen);
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);
    is_connected = connect(sockfd,(struct sockaddr *) &serveraddr, serverlen) >= 0;
    if (!is_connected) error("ERROR connecting", true);
}

void TCPclient::send(const unsigned char *msg, const int len)
{
	//cout << "sending to port " << portno << endl;
    if (write(sockfd, msg, len) < 0) error("ERROR writing to socket", false);
    //cout << "sent to port " << portno << endl;
}

void TCPclient::send(const char *msg, const int len)
{
	//cout << "sending to port " << portno << endl;
    if (write(sockfd, msg, len) < 0) error("ERROR writing to socket", false);
    fsync(sockfd);
    //cout << "sent to port " << portno << endl;
}

unsigned char *TCPclient::recv(const int len)
{
    unsigned char *msg = new unsigned char[len];
	if (read(sockfd, msg, len) < 0) error("ERROR reading from socket", false);
	return msg;
}

void TCPclient::recv(char *msg, int len)
{
    if (read(sockfd, msg, len) < 0) error("ERROR reading from socket", false);
}

void TCPclient::closeSocket()
{
	close(sockfd);
}

TCPclient::~TCPclient(){}

SocketClient *createClient(const string protocol, const string host_name, const int port_no, const bool immediate_connect)
{
	SocketClient *client;
	if(protocol == "UDP")
		client = new UDPclient(host_name.c_str(), port_no);
	else
		client = new TCPclient(host_name.c_str(), port_no);
	if(immediate_connect)
	{
		//cout << "trying to connect to " << protocol << "|" << host_name << ":" << port_no << "..." << endl;
		client->connect2Server();
		/*if(client->is_connected)
			cout << "connected to " << protocol << "|" << host_name << ":" << port_no << endl;*/
	}
	return client;
}
