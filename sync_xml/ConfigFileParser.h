#ifndef _CONFIG_FILE_PARSER_H
#define _CONFIG_FILE_PARSER_H

#include <fstream>
#include <map>
#include <utility>
#include <string>
#include <stdlib.h>
#include <exception>

class ConfigFileParser
{
	private:
	
		std::map<std::string, std::string> config_map;
		std::string path;
		char splitter;
		std::ifstream file_stream;
		
		void parseFile();

	public:

		ConfigFileParser();
		ConfigFileParser(std::string _path, char _splitter);
		std::string getStrValueByName(std::string name);
		int getIntValueByName(std::string name);
		bool getBoolValueByName(std::string name);
		int getSize();
		~ConfigFileParser();
};

#endif
