#include <iostream>

#include "SyncXmlParser.h"

using namespace std;

SyncXmlParser::SyncXmlParser(){}

SyncXmlParser::SyncXmlParser(int argc, char* argv[])
{
	default_xml_file_name = "sync.xml";
	xml_file_name = default_xml_file_name;
	if(argc > 1)
	{
		xml_file_name = argv[1];
	}
    setXmlFile();
    parseXmlFile();
}

bool SyncXmlParser::isFileOK()
{
	ifstream f(xml_file_name.c_str());
	bool file_ok = f.good();
    f.close();
    return file_ok;
}

void SyncXmlParser::setXmlFile()
{
	if(!isFileOK())
    {
    	xml_file_name = default_xml_file_name;
    	if(!isFileOK())
    	{
    		printDebugMessage("xml file was not found: " + xml_file_name, ERROR);
    		exit(1);
    	}
    }
   	printDebugMessage("xml file name: " + xml_file_name, INFO);
}

void SyncXmlParser::parseXmlFile()
{
	printDebugMessage("parsing xml file", STATUS);
	TiXmlDocument doc(xml_file_name.c_str());
	bool load_ok = doc.LoadFile();
	if(load_ok)
	{
		printDebugMessage("file loaded: " + xml_file_name, INFO);
	}
	else
	{
		printDebugMessage("file load error: " + xml_file_name, INFO);
		exit(1);

	}
	TiXmlHandle doc_handle(&doc);
	TiXmlHandle sync = doc_handle.ChildElement("sync", 0);
	if(sync.Element())
	{
		TiXmlHandle streams = sync.ChildElement("streams", 0);
		TiXmlHandle general = sync.ChildElement("general", 0);
		if(streams.Element())
		{
			TiXmlHandle dvb = streams.ChildElement("dvb", 0);
			cout << dvb.ChildElement("active", 0).Element()->Value() << ": " << dvb.ChildElement("active", 0).Element()->GetText() << endl;

			int number_of_p2ps = 0;
			for(int i=0; i<30; i++)
			{
				TiXmlHandle p2p = streams.ChildElement("p2p", i);
				if(p2p.Element())
				{
					number_of_p2ps++;
				}
				else
				{
					break;
				}
			}
			cout << "number of p2p streams: " << number_of_p2ps << endl;
		}
		if(general.Element())
		{

		}
	}
}

SyncXmlParser::~SyncXmlParser(){}