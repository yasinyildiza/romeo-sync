#include <iostream>

#include "debugMessagePrinter.h"

using namespace std;

int DEBUG_LEVEL = WARNING;

bool isIn(string a, string b)
{
	if(a.length() < b.length())
		return false;
	if(a.length() == b.length())
		return a == b;
	for(unsigned int i = 0; i < a.length() - b.length(); i++)
	{
		for(unsigned int j = 0; j < b.length(); j++)
		{
			if(a[i+j] != b[i+j])
				break;
			if(j == b.length() - 1)
				return true;
		}
	}
	return false;
}

string getDebugLevelStr()
{
	if(DEBUG_LEVEL == ERROR)
		return "ERROR";
	else if(DEBUG_LEVEL == WARNING)
		return "WARNING";
	else if(DEBUG_LEVEL == STATUS)
		return "STATUS";
	else if(DEBUG_LEVEL == INFO)
		return "INFO";
	else if(DEBUG_LEVEL == LOOP)
		return "LOOP";
	return "BEYOND";
}

void setDebugLevel(int argc, char* argv[])
{
	if(argc > 1)
		DEBUG_LEVEL = (debug_level)atoi(argv[1]);
	printDebugMessage("DEBUG LEVEL has been set to " + getDebugLevelStr(), STATUS);
	if(print_as_hex)
		cout << hex << endl;
}

void printDebugMessage(const char *msg, const debug_level priority)
{
	if(priority <= DEBUG_LEVEL)
		cout << msg << endl;
}

void printDebugMessage(const string msg, const debug_level priority)
{
	if(priority <= DEBUG_LEVEL)
		cout << msg << endl;
}

void printDebugMessage(const char *msg)
{
	if(DEBUG_LEVEL >= LOOP)
		cout << msg << endl;
}

void printDebugMessage(const string msg)
{
	if(DEBUG_LEVEL >= LOOP)
		cout << msg << endl;
}

void writeDebugMessage(const string file_name, const string msg, const debug_level priority)
{
	if(priority <= DEBUG_LEVEL)
	{
		ofstream debug_file;
		debug_file.open (("log/" + file_name).c_str(), ios::app);
		debug_file << msg << endl;
		debug_file.close();
	}
}

void printRawData(const unsigned char *data, const int length)
{
	for(int i=0; i<length; i++)
	{
		cout << hex << (int)data[i];
		if(i != length - 1)
			cout << " ";
	}
	cout << endl;
}

void writeRawData(const string file_name, const unsigned char *data, const int length)
{
	ofstream output_file;
	output_file.open (("log/" + file_name).c_str(), ios::app);
	output_file.write((const char*)data, length);
	output_file.close();
	printDebugMessage(to_str(length) + " bytes have been written to " + file_name, BEYOND);
}

//-----------------------------------------------------------------------------
// Format current time (calculated as an offset in current day) in this form:
//
//     "hh:mm:ss.SSS" (where "SSS" are milliseconds)
//-----------------------------------------------------------------------------
string now_str()
{
    // Get current time from the clock, using microseconds resolution
    const boost::posix_time::ptime now = 
        boost::posix_time::microsec_clock::local_time();

    // Get the time offset in current day
    const boost::posix_time::time_duration td = now.time_of_day();

    //
    // Extract hours, minutes, seconds and milliseconds.
    //
    // Since there is no direct accessor ".milliseconds()",
    // milliseconds are computed _by difference_ between total milliseconds
    // (for which there is an accessor), and the hours/minutes/seconds
    // values previously fetched.
    //
    const long hours        = td.hours();
    const long minutes      = td.minutes();
    const long seconds      = td.seconds();
    const long milliseconds = td.total_milliseconds() -
                              ((hours * 3600 + minutes * 60 + seconds) * 1000);

    //
    // Format like this:
    //
    //      hh:mm:ss.SSS
    //
    // e.g. 02:15:40:321
    //
    //      ^          ^
    //      |          |
    //      123456789*12
    //      ---------10-     --> 12 chars + \0 --> 13 chars should suffice
    //  
    // 
    char buf[40];
    sprintf(buf, "%02ld:%02ld:%02ld.%03ld", 
        hours, minutes, seconds, milliseconds);

    return buf;
}

long long int getMicroSeconds()
{
    const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    const boost::posix_time::time_duration td = now.time_of_day();
    return td.total_microseconds();
}
