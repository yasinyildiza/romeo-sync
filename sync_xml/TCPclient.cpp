#include <iostream>

#include "TCPclient.h"

using namespace std;

TCPclient::TCPclient(const std::string host_name, const int port_no)
{
	hostname = host_name;
	portno = port_no;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket", true);
}

bool TCPclient::connect2Server()
{
	server = gethostbyname(hostname.c_str());
    if (server == NULL) error("ERROR, no such host", true);

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    bool connected = connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) >= 0;
    if (connected)
        return true;
    error("ERROR connecting", true);
    return false;
}

void TCPclient::send(const unsigned char *msg, const int len)
{
    if (write(sockfd, msg, len) < 0) error("ERROR writing to socket", false);
}

void TCPclient::send(const char *msg, const int len)
{
    if (write(sockfd, msg, len) < 0) error("ERROR writing to socket", false);
    fsync(sockfd);
}

unsigned char *TCPclient::recv()
{
    unsigned char *msg = new unsigned char[188];
	if (read(sockfd, msg, 188) < 0) error("ERROR reading from socket", false);
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

void TCPclient::error(const char *msg, const bool high)
{
    cout << hostname << ": " << portno << " " << msg << " " << errno << endl;
}

TCPclient::~TCPclient(){}
