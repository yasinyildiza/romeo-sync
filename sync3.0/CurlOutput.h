#ifndef _CURL_OUTPUT_H
#define _CURL_OUTPUT_H

#include <string.h>
#include <curl/curl.h>
#include "tinyxml.h"

class CurlOutput
{
	private:

	public:
		char *bytes;
		int length;
		TiXmlDocument *xml_doc;

		CurlOutput();
		CurlOutput(char *_bytes, int _length);
		CurlOutput(CurlOutput &rhs);
		CurlOutput &operator=(CurlOutput &rhs);
		void processUrl(std::string url);
		~CurlOutput();
};

size_t curlWriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

#endif
