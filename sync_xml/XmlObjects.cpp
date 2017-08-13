#include <iostream>

#include "XmlObjects.h"

using namespace std;

XmlObjectBase::XmlObjectBase()
{
	active = false;
}

XmlObjectBase::~XmlObjectBase()
{}

XmlDvbObject::XmlDvbObject()
{}

XmlDvbObject::XmlDvbObject(TiXmlHandle *handler)
{}

void XmlDvbObject::print()
{}

XmlDvbObject::~XmlDvbObject()
{}