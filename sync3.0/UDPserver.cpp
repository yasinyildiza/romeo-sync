#include <iostream>

#include "UDPserver.h"

using namespace std;

UDPserver::UDPserver(int port_no)
{
	portno = port_no;
	
	/*
	 * socket: create the parent socket  
	 */
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) error("ERROR opening socket", true);
	
	/* 
	 * setsockopt: Handy debugging trick that lets 
	 * us rerun the server immediately after we kill it; 
	 * otherwise we have to wait about 20 secs. 
	 * Eliminates "ERROR on binding: Address already in use" error. 
	 */
	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
	//setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const void *)&optval , 64000);
	
	/*
	 * build the server's Internet address
	 */
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)portno);
	
	/*
	 * bind: associate the parent socket with a port 
	 */
	if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) error("ERROR on binding", true);
	
	clientlen = sizeof(clientaddr);
}

void UDPserver::recv(unsigned char *msg, const int len)
{
    n = recvfrom(sockfd, msg, len, 0, (struct sockaddr *) &clientaddr, &clientlen);
    if(n < 0) error("ERROR in recvfrom", false);
}

int UDPserver::getLastMessageLength()
{
	return n;
}

void UDPserver::send(const unsigned char *msg, const int len)
{
	n = sendto(sockfd, msg, len, 0, (struct sockaddr *) &clientaddr, clientlen);
    if(n < 0) error("ERROR in sendto", false);
}

void UDPserver::error(const char *msg, const bool high)
{
	cout << "host: " << portno << " " << msg << " " << errno << endl;
}

UDPserver::~UDPserver(){}
