#ifndef _SYNC_XML_PARSER_H
#define _SYNC_XML_PARSER_H

#include <fstream>
#include <string>
#include <cstdlib>
#include "debugMessagePrinter.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "XmlObjects.h"

class SyncXmlParser
{
	private:
		std::string default_xml_file_name;
		std::string xml_file_name;

	public:
		SyncXmlParser();
		SyncXmlParser(int argc, char* argv[]);
		bool isFileOK();
		void setXmlFile();
		void parseXmlFile();
		~SyncXmlParser();
};

#endif