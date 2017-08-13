#ifndef _XML_OBJECTS_H
#define _XML_OBJECTS_H

#include "tinystr.h"
#include "tinyxml.h"

class XmlObjectBase
{
	private:

	public:
		bool active;

		XmlObjectBase();
		virtual void print() = 0;
		virtual ~XmlObjectBase();

};

class XmlStreamObject : public XmlObjectBase
{
	private:

	public:
		XmlStreamObject();
		~XmlStreamObject();

};

class XmlDvbObject : public XmlObjectBase
{
	private:

	public:
		XmlDvbObject();
		XmlDvbObject(TiXmlHandle *handler);
		void print();
		~XmlDvbObject();
};

#endif