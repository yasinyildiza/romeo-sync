#include <iostream>

#include "CurlOutput.h"

using namespace std;

CurlOutput::CurlOutput()
{
	bytes = NULL;
	length = 0;
	xml_doc = NULL;
}

CurlOutput::CurlOutput(char *_bytes, int _length)
{
	length = _length;
	bytes = new char[length];
	memcpy(bytes, _bytes, length);
}

CurlOutput::CurlOutput(CurlOutput &rhs)
{
	length = rhs.length;
	bytes = new char[length];
	memcpy(bytes, rhs.bytes, length);
	/*xml_doc = new TiXmlDocument();
	*xml_doc = *(rhs.xml_doc);*/
}

CurlOutput &CurlOutput::operator=(CurlOutput &rhs)
{
	delete []bytes;
	delete xml_doc;
	length = rhs.length;
	bytes = new char[length];
	memcpy(bytes, rhs.bytes, length);
	/*xml_doc = new TiXmlDocument("ROMEO.xml");
	xml_doc = *(rhs.xml_doc);*/

	return *this;
}

void CurlOutput::processUrl(string url)
{
	CURL *curl_handler;
	CURLcode curl_response;
	curl_global_init(CURL_GLOBAL_ALL);
	curl_handler = curl_easy_init();
	if(curl_handler)
	{
		curl_easy_setopt(curl_handler, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl_handler, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl_handler, CURLOPT_WRITEFUNCTION, curlWriteCallback);
		curl_easy_setopt(curl_handler, CURLOPT_WRITEDATA, (void *)this);
		curl_response = curl_easy_perform(curl_handler);
		if(curl_response != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(curl_response));
		}
		else
		{
			xml_doc = new TiXmlDocument("ROMEO.xml");
			xml_doc->Parse(bytes);
			if(xml_doc->Error())
			{
				printf("Error in %s: %s\n", xml_doc->Value(), xml_doc->ErrorDesc());
			}
			xml_doc->SaveFile();
		}
		curl_easy_cleanup(curl_handler);
	}
	else
	{
		printf("curl_handle could not be initiated\n");
	}
	curl_global_cleanup();
}

CurlOutput::~CurlOutput()
{
	delete []bytes;
	delete xml_doc;
}

size_t curlWriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	CurlOutput *output = (CurlOutput *)userp;
	CurlOutput *temp = new CurlOutput();
	temp->bytes = new char[output->length + realsize];
	memcpy(temp->bytes + temp->length, output->bytes, output->length);
	temp->length += output->length;
	memcpy(temp->bytes + temp->length, (char*)contents, realsize);
	temp->length += realsize;
	*output = *temp;
	delete temp;
	return realsize;
}
