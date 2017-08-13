#include <iostream>
#include <stdlib.h>
#include <errno.h>

#include "UDPclient.h"

using namespace std;

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

bool UDPclient::connect2Server()
{
	return true;
}

unsigned char *UDPclient::recv(const int len)
{
	unsigned char *msg = new unsigned char[len];
	int n = recvfrom(sockfd, (void *)msg, len, 0, (sockaddr*)&serveraddr, &serverlen);
	if(n < 0) error("ERROR in recv", false);
	return msg;
}

void UDPclient::send(const unsigned char *msg, const int len)
{
	int number_of_send = len / MAXIMUM_DATAGRAM_SIZE;
	if(number_of_send > 1)
		cout << "number of send: " << number_of_send << endl;
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

void UDPclient::error(const char *msg, const bool high)
{
	cout << hostname << ": " << portno << " " << msg << " " << errno << endl;
}

UDPclient::~UDPclient(){}
