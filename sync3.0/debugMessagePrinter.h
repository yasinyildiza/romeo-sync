#ifndef _DEBUG_MESSAGE_PRINTER_H
#define _DEBUG_MESSAGE_PRINTER_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "globals.h"

typedef enum debug_level
{
	ERROR,
	WARNING,
	STATUS,
	INFO,
	LOOP,
	BEYOND
} debug_level_t;

template <class T>
std::string to_str(T value)
{
	return std::to_string((long long)value);
}
bool isIn();
std::string getDebugLevelStr();
void setDebugLevel(int argc, char* argv[]);
void printDebugMessage(const char *msg, const debug_level priority);
void printDebugMessage(const std::string msg, const debug_level priority);
void printDebugMessage(const char *msg);
void printDebugMessage(const std::string msg);
void writeDebugMessage(const std::string file_name, const std::string msg, const debug_level priority);
void printRawData(const unsigned char *data, const int length);
void writeRawData(const std::string file_name, const unsigned char *data, const int length);
std::string now_str();
long long int getMicroSeconds();

template <class T>
void printAttribute(std::string name, T value)
{
	std::cout << "\t" << name << ": " << value << std::endl;
}

template <class T>
void printArrayAttribute(std::string name, T* value, const int length)
{
	/*std::cout << "\t" << name << ": ";
	for(int i=0; i<length; i++)
	{
		std::cout << (unsigned short int)value[i] << " ";
	}
	std::cout << std::endl;*/
	
	std::cout << "\t" << name << ": ";
	for(int i=0; i<length; i++)
	{
		std::cout << value[i];
	}
	std::cout << std::endl;
}

#endif
