#include <iostream>

#include "SyncXmlParser.h"

using namespace std;

int main(int argc, char* argv[])
{
	setDebugLevel(argc, argv);
	printDebugMessage("START OF PROGRAM", WARNING);
	SyncXmlParser *sync_xml_parser = new SyncXmlParser(argc, argv);
	delete sync_xml_parser;
	printDebugMessage("END OF PROGRAM", WARNING);
	return 0;
}
