#include <iostream>

#include "ConfigFileParser.h"
#include "debugMessagePrinter.h"

using namespace std;

ConfigFileParser::ConfigFileParser(){}

ConfigFileParser::ConfigFileParser(string _path, char _splitter)
{
	path = _path;
	splitter = _splitter;
	
	file_stream.open(path.c_str());
	parseFile();
	file_stream.close();
}

void ConfigFileParser::parseFile()
{
	if(!file_stream.good())
	{
		printDebugMessage("file stream error: " + path, ERROR);
		return;
	}
	
	string line;
	string label = "";
	string value = "";
	unsigned int i = 0;
	unsigned int line_no = 1;
	
	while(file_stream.good())
	{
		getline(file_stream, line);
		
		if(line.length() == 0)
		{
			//printDebugMessage("empty line (" + to_str(line_no) + ") skipped in the config file: " + path, INFO);
			continue;
		}
		
		if(line.length() < 3)
		{
			printDebugMessage("invalid line (" + to_str(line_no) + ") skipped in the config file: " + path, WARNING);
			continue;
		}
		
		while(line[i] != splitter)
		{
			if(i == line.length() - 1)
			{
				printDebugMessage("invalid config file: " + path, ERROR);
				return;
			}
			
			if(line[i] == ' ' || line[i] == '\t')
			{
				i++;
				continue;
			}
			
			label += line[i];
			i++;
		}
		
		i++;	//splitter
		
		while(i<line.length())
		{
			if(line[i] == ' ' || line[i] == '\t')
			{
				i++;
				continue;
			}
			
			value += line[i];
			i++;
		}
		config_map[label] = value;
		line_no++;
		i = 0;
		label = "";
		value = "";
	}
}

int ConfigFileParser::getSize()
{
	return config_map.size();
}

string ConfigFileParser::getStrValueByName(string name)
{
	return config_map[name];
}

int ConfigFileParser::getIntValueByName(string name)
{
	return atoi(config_map[name].c_str());
}

bool ConfigFileParser::getBoolValueByName(string name)
{
	return config_map[name] == "true";
}

ConfigFileParser::~ConfigFileParser(){}
