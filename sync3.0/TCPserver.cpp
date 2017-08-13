#include <iostream>

#include "TCPserver.h"

using namespace std;

TCPserver::TCPserver(const int port_no)
{
	portno = port_no;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) error("ERROR opening socket", true);

	bzero((char *) &serv_addr, sizeof(serv_addr));	//what the ... is that?

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
}

void TCPserver::acceptClient()
{
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) error("ERROR on accept", true);
}

bool TCPserver::recv(char *msg, int len)
{
	int ret = read(newsockfd, msg, len);
	if(ret < 0) error("ERROR reading from socket", false);
	return ret;
}

void TCPserver::send(const char *msg)
{
	if (write(newsockfd, msg, strlen(msg)) < 0) error("ERROR writing to socket", false);
}

void TCPserver::closeSocket()
{
	close(newsockfd);
	close(sockfd);
}

void TCPserver::error(const char *msg, const bool high)
{
	cout << "host: " << portno << " " << msg << " " << errno << endl;
}

TCPserver::~TCPserver(){}
