#include <iostream>
#include <stdio.h>
#include <string.h>

#include "TsPacket.h"
#include "debugMessagePrinter.h"

using namespace std;

TsDescriptorObject::TsDescriptorObject()
{
	descriptor = NULL;
}

TsDescriptorObject::TsDescriptorObject(unsigned short int offset, unsigned char bytes[188])
{
	descriptor_tag = bytes[offset++];
	descriptor_length = bytes[offset++];
	descriptor = new char[descriptor_length];
	char byte;
	for(int i=0; i<descriptor_length; i++)
	{
		if(offset == TS_PACKET_SIZE)
			break;
		byte = bytes[offset++];
		descriptor[i] = ((byte == '\0') ? ' ' : byte);
	}
}

TsDescriptorObject::TsDescriptorObject(TsDescriptorObject &rhs)
{
	descriptor_tag = rhs.descriptor_tag;
	descriptor_length = rhs.descriptor_length;
	descriptor = new char[descriptor_length];
	memcpy(descriptor, rhs.descriptor, descriptor_length);
}

TsDescriptorObject &TsDescriptorObject::operator=(TsDescriptorObject &rhs)
{
	delete []descriptor;
	descriptor_tag = rhs.descriptor_tag;
	descriptor_length = rhs.descriptor_length;
	descriptor = new char[descriptor_length];
	memcpy(descriptor, rhs.descriptor, descriptor_length);
	return *this;
}

void TsDescriptorObject::print()
{
	printDebugMessage("TS descriptor attributes:", INFO);
	printAttribute("descriptor_tag", (int)descriptor_tag);
	printAttribute("descriptor_length", (int)descriptor_length);
	if(descriptor_tag == 0x48)
	{
		unsigned short int offset = 0;
		unsigned char service_type = descriptor[offset++];
		unsigned char service_provider_name_length = descriptor[offset++];
		char *service_provider_name = new char[service_provider_name_length];
		char byte;
		for(int i=0; i<service_provider_name_length; i++)
		{
			byte = descriptor[offset++];
			service_provider_name[i] = ((byte == '\0') ? ' ' : byte);
		}
		unsigned char service_name_length = descriptor[offset++];
		char *service_name = new char[service_name_length];
		for(int i=0; i<service_name_length; i++)
		{
			byte = descriptor[offset++];
			service_name[i] = ((byte == '\0') ? ' ' : byte);
		}
		printAttribute("\tservice_type", (int)service_type);
		printAttribute("\tservice_provider_name_length", (int)service_provider_name_length);
		printArrayAttribute("\tservice_provider_name", service_provider_name, service_provider_name_length);
		printAttribute("\tservice_name_length", (int)service_name_length);
		printArrayAttribute("\tservice_name", service_name, service_name_length);
	}
	else
	{
		printArrayAttribute("descriptor", descriptor, descriptor_length);
	}
}

TsDescriptorObject::~TsDescriptorObject()
{
	delete []descriptor;
}

PatProgramObject::PatProgramObject(){}

PatProgramObject::PatProgramObject(unsigned short int offset, unsigned char bytes[188])
{
	program_no1 = bytes[offset++] << 8;
	program_no2 = bytes[offset++];
	program_no = program_no1 | program_no2;
	
	empty_bits = (bytes[offset] & 0xE0) >> 5;
	
	pmt_pid1 = (bytes[offset++] & 0x1F) << 8;
	pmt_pid2 = bytes[offset++];
	pmt_pid = pmt_pid1 | pmt_pid2;
}

void PatProgramObject::print()
{
	printDebugMessage("PAT program attributes:", INFO);
	printAttribute("program_no", (int)program_no);
	printAttribute("empty_bits", (int)empty_bits);
	printAttribute("pmt_pid", (int)pmt_pid);
}

PatProgramObject::~PatProgramObject(){}

EitEventObject::EitEventObject()
{
	descriptor_list = NULL;
}

EitEventObject::EitEventObject(unsigned short int offset, unsigned char bytes[188])
{
    descriptor_list = new TsDescriptorObject[30];
    descriptor_list_length = 0;

    event_id1 = bytes[offset++] << 8;
    event_id2 = bytes[offset++];
    event_id = event_id1 | event_id2;

    start_time1 = bytes[offset++] << 24;
    start_time2 = bytes[offset++] << 24;
    start_time3 = bytes[offset++] << 16;
    start_time4 = bytes[offset++] << 8;
    start_time5 = bytes[offset++];
    start_time  = start_time1 | start_time2 | start_time3 | start_time4 | start_time5;

    duration1 = bytes[offset++] << 16;
    duration2 = bytes[offset++] << 8;
    duration3 = bytes[offset++];
    duration = duration1 | duration2 | duration3;

    running_status = (bytes[offset] & 0xE0) >> 5;

    free_CA_mode = bytes[offset] & 0x10;

    descriptors_loop_length1 = (bytes[offset++] & 0x0F) << 8;
    descriptors_loop_length2 = bytes[offset++];
    descriptors_loop_length = descriptors_loop_length1 | descriptors_loop_length2;

    //N loop for descriptors
    unsigned char info_cnt = 0;
    TsDescriptorObject *current_descriptor;
    while(info_cnt < descriptors_loop_length)
    {
		if(offset >= TS_PACKET_SIZE - TS_CRC32_SIZE)
			break;
        current_descriptor = new TsDescriptorObject(offset, bytes);
        descriptor_list[descriptor_list_length++] = *current_descriptor;
        info_cnt += 2 + current_descriptor->descriptor_length;
        offset += 2 + current_descriptor->descriptor_length;
        delete current_descriptor;
    }
}

EitEventObject::EitEventObject(EitEventObject &rhs)
{
    event_id = rhs.event_id;
    start_time = rhs.start_time;
    duration = rhs.duration;
    running_status = rhs.running_status;
    free_CA_mode = rhs.free_CA_mode;
    descriptor_list_length = rhs.descriptor_list_length;
    descriptors_loop_length = rhs.descriptors_loop_length;

    descriptor_list = new TsDescriptorObject[rhs.descriptor_list_length];
    for(unsigned int i=0; i<rhs.descriptor_list_length; i++)
    {
        descriptor_list[i] = rhs.descriptor_list[i];
    }
}

EitEventObject &EitEventObject::operator=(EitEventObject &rhs)
{
    event_id = rhs.event_id;
    start_time = rhs.start_time;
    duration = rhs.duration;
    running_status = rhs.running_status;
    free_CA_mode = rhs.free_CA_mode;
    descriptor_list_length = rhs.descriptor_list_length;
    descriptors_loop_length = rhs.descriptors_loop_length;

	delete []descriptor_list;
    descriptor_list = new TsDescriptorObject[rhs.descriptor_list_length];
    for(unsigned int i=0; i<rhs.descriptor_list_length; i++)
    {
        descriptor_list[i] = rhs.descriptor_list[i];
    }
    return *this;
}

unsigned short int EitEventObject::getTotalLength()
{
	unsigned short int total_length = 12;
	
	for(unsigned int i=0; i<descriptor_list_length; i++)
	{
		total_length += 2 + descriptor_list[i].descriptor_length;
	}
	
	return total_length;
}

void EitEventObject::print()
{
	printDebugMessage("TS EIT event attributes:", INFO);
	printAttribute("event_id", (int)event_id);
	printAttribute("start_time", (int)start_time);
	printAttribute("duration", (int)duration);
	printAttribute("running_status", (int)running_status);
	printAttribute("free_CA_mode", (int)free_CA_mode);
	printAttribute("descriptors_loop_length", (int)descriptors_loop_length);
	printAttribute("descriptor_list_length", (int)descriptor_list_length);
    for(unsigned short int i=0; i<descriptor_list_length; i++)
    {
        descriptor_list[i].print();
    }
}

EitEventObject::~EitEventObject()
{
	delete []descriptor_list;
}

BatTransportStreamObject::BatTransportStreamObject()
{
	descriptor_list = NULL;
}

BatTransportStreamObject::BatTransportStreamObject(unsigned short int offset, unsigned char bytes[188])
{
    descriptor_list = new TsDescriptorObject[30];
    descriptor_list_length = 0;

    transport_stream_id1 = bytes[offset++] << 8;
    transport_stream_id2 = bytes[offset++];
    transport_stream_id = transport_stream_id1 | transport_stream_id2;

    original_network_id1 = bytes[offset++] << 8;
    original_network_id2 = bytes[offset++];
    original_network_id  = original_network_id1 | original_network_id2;
    
    reserved_for_future_purpose_bits = (bytes[offset] & 0xF0) >> 4;
    
    descriptors_loop_length1 = (bytes[offset++] & 0x0F) << 8;
    descriptors_loop_length2 = bytes[offset++];
    descriptors_loop_length = descriptors_loop_length1 | descriptors_loop_length2;

    //N loop for descriptors
    unsigned char info_cnt = 0;
    TsDescriptorObject *current_descriptor;
    while(info_cnt < descriptors_loop_length)
    {
		if(offset >= TS_PACKET_SIZE - TS_CRC32_SIZE)
			break;
        current_descriptor = new TsDescriptorObject(offset, bytes);
        descriptor_list[descriptor_list_length++] = *current_descriptor;
        info_cnt += 2 + current_descriptor->descriptor_length;
        offset += 2 + current_descriptor->descriptor_length;
        delete current_descriptor;
    }
}

BatTransportStreamObject::BatTransportStreamObject(BatTransportStreamObject &rhs)
{
    transport_stream_id = rhs.transport_stream_id;
    original_network_id = rhs.original_network_id;
    reserved_for_future_purpose_bits = rhs.reserved_for_future_purpose_bits;
    descriptor_list_length = rhs.descriptor_list_length;
    descriptors_loop_length = rhs.descriptors_loop_length;

    descriptor_list = new TsDescriptorObject[rhs.descriptor_list_length];
    for(unsigned int i=0; i<rhs.descriptor_list_length; i++)
    {
        descriptor_list[i] = rhs.descriptor_list[i];
    }
}

BatTransportStreamObject &BatTransportStreamObject::operator=(BatTransportStreamObject &rhs)
{
    transport_stream_id = rhs.transport_stream_id;
    original_network_id = rhs.original_network_id;
    reserved_for_future_purpose_bits = rhs.reserved_for_future_purpose_bits;
    descriptor_list_length = rhs.descriptor_list_length;
    descriptors_loop_length = rhs.descriptors_loop_length;
    
	delete []descriptor_list;
    descriptor_list = new TsDescriptorObject[rhs.descriptor_list_length];
    for(unsigned int i=0; i<rhs.descriptor_list_length; i++)
    {
        descriptor_list[i] = rhs.descriptor_list[i];
    }
    return *this;
}

unsigned short int BatTransportStreamObject::getTotalLength()
{
	unsigned short int total_length = 48;
	
	for(unsigned int i=0; i<descriptor_list_length; i++)
	{
		total_length += 2 + descriptor_list[i].descriptor_length;
	}
	
	return total_length;
}

void BatTransportStreamObject::print()
{
	printDebugMessage("TS BAT Transport Stream attributes:", INFO);
	printAttribute("transport_stream_id", (int)transport_stream_id);
	printAttribute("original_network_id", (int)original_network_id);
	printAttribute("reserved_for_future_purpose_bits", (int)reserved_for_future_purpose_bits);
	printAttribute("descriptors_loop_length", (int)descriptors_loop_length);
	printAttribute("descriptor_list_length", (int)descriptor_list_length);
    for(unsigned short int i=0; i<descriptor_list_length; i++)
    {
        descriptor_list[i].print();
    }
}

BatTransportStreamObject::~BatTransportStreamObject()
{
	delete []descriptor_list;
}


PmtElementaryStreamObject::PmtElementaryStreamObject()
{
	descriptor_list = NULL;
}

PmtElementaryStreamObject::PmtElementaryStreamObject(unsigned short int offset, unsigned char bytes[188])
{
	descriptor_list_length = 0;
	descriptor_list = new TsDescriptorObject[10];
	
	stream_type = bytes[offset++];
	reserved_bits1 = (bytes[offset] & 0xE0) >> 5;
	
	elementary_pid1 = (bytes[offset++] & 0x1F) << 8;
	elementary_pid2 = bytes[offset++];
	elementary_pid = elementary_pid1 | elementary_pid2;
	
	reserved_bits2 = (bytes[offset] & 0xF0) >> 4;
	
	es_info_length1 = (bytes[offset++] & 0x0F) << 8;
	es_info_length2 = bytes[offset++];
	es_info_length = es_info_length1 | es_info_length2;
	
	//N loop for descriptors
	unsigned char info_cnt = 0;
	TsDescriptorObject *current_descriptor;
	while(info_cnt < es_info_length)
	{
		if(offset >= TS_PACKET_SIZE - TS_CRC32_SIZE)
			break;
		current_descriptor = new TsDescriptorObject(offset, bytes);
		descriptor_list[descriptor_list_length++] = *current_descriptor;
		info_cnt += 2 + current_descriptor->descriptor_length;
		offset += 2 + current_descriptor->descriptor_length;
		delete current_descriptor;
	}
}

PmtElementaryStreamObject::PmtElementaryStreamObject(PmtElementaryStreamObject &rhs)
{
	stream_type = rhs.stream_type;
	reserved_bits1 = rhs.reserved_bits1;
	elementary_pid = rhs.elementary_pid;
	reserved_bits2 = rhs.reserved_bits2;
	es_info_length = rhs.es_info_length;
	descriptor_list_length = rhs.descriptor_list_length;
	
	descriptor_list = new TsDescriptorObject[rhs.descriptor_list_length];
	for(unsigned int i=0; i<rhs.descriptor_list_length; i++)
	{
		descriptor_list[i] = rhs.descriptor_list[i];
	}
}

PmtElementaryStreamObject &PmtElementaryStreamObject::operator=(PmtElementaryStreamObject &rhs)
{
	stream_type = rhs.stream_type;
	reserved_bits1 = rhs.reserved_bits1;
	elementary_pid = rhs.elementary_pid;
	reserved_bits2 = rhs.reserved_bits2;
	es_info_length = rhs.es_info_length;
	descriptor_list_length = rhs.descriptor_list_length;
	
	delete []descriptor_list;
	descriptor_list = new TsDescriptorObject[rhs.descriptor_list_length];
	for(unsigned int i=0; i<rhs.descriptor_list_length; i++)
	{
		descriptor_list[i] = rhs.descriptor_list[i];
	}
	
	return *this;
}

void PmtElementaryStreamObject::print()
{
	printDebugMessage("PMT elementary stream attributes:", INFO);
	printAttribute("stream_type", (int)stream_type);
	printAttribute("reserved_bits1", (int)reserved_bits1);
	printAttribute("elementary_pid", (int)elementary_pid);
	printAttribute("reserved_bits2", (int)reserved_bits2);
	printAttribute("es_info_length", (int)es_info_length);
	for(unsigned int i=0; i<descriptor_list_length; i++)
	{
		descriptor_list[i].print();
	}
}

PmtElementaryStreamObject::~PmtElementaryStreamObject()
{
	delete []descriptor_list;
}

SdtServiceObject::SdtServiceObject()
{
	descriptor_list = NULL;
	additional_content_url_descriptor = NULL;
}

SdtServiceObject::SdtServiceObject(unsigned short int offset, unsigned char bytes[188])
{
	descriptor_list_length = 0;
	descriptor_list = NULL;
	descriptor_list = new TsDescriptorObject[30];

	additional_content_url_descriptor = NULL;

	service_id1 = bytes[offset++] << 8;
	service_id2 = bytes[offset++];
	service_id = service_id1 | service_id2;

	reserved_bits1 = (bytes[offset] & 0xFC) >> 2;
	EIT_schedule_flag = bytes[offset] & 0x02;
	EIT_present_following_flag = bytes[offset++] & 0x02;
	running_status = bytes[offset] & 0xE0;
	free_CA_mode = bytes[offset] & 0x10;
	descriptors_loop_length1 = (bytes[offset++] & 0x0F) << 8;
	descriptors_loop_length2 = bytes[offset++];
	descriptors_loop_length = descriptors_loop_length1 | descriptors_loop_length2;

	//N loop for descriptors
	unsigned char info_cnt = 0;
	TsDescriptorObject *current_descriptor;
	while(info_cnt < descriptors_loop_length)
	{
		if(offset >= TS_PACKET_SIZE - TS_CRC32_SIZE)
			break;
		current_descriptor = new TsDescriptorObject(offset, bytes);
		if(current_descriptor->descriptor_tag == EXTENSION_DESCRIPTOR && additional_content_url_flag)
		{
			additional_content_url_descriptor = new TsAdditionalContentUrlDescriptorObject(offset, bytes);
		}
		descriptor_list[descriptor_list_length++] = *current_descriptor;
		info_cnt += 2 + current_descriptor->descriptor_length;
		offset += 2 + current_descriptor->descriptor_length;
		delete current_descriptor;
	}
}

SdtServiceObject::SdtServiceObject(SdtServiceObject &rhs)
{
	service_id = rhs.service_id;
	reserved_bits1 = rhs.reserved_bits1;
	EIT_schedule_flag = rhs.EIT_schedule_flag;
	EIT_present_following_flag = rhs.EIT_present_following_flag;
	running_status = rhs.running_status;
	free_CA_mode = rhs.free_CA_mode;
	descriptors_loop_length = rhs.descriptors_loop_length;
	descriptor_list_length = rhs.descriptor_list_length;

	if(additional_content_url_flag)
	{
		*additional_content_url_descriptor = *(rhs.additional_content_url_descriptor);
	}
	
	descriptor_list = new TsDescriptorObject[rhs.descriptor_list_length];
	for(unsigned int i=0; i<rhs.descriptor_list_length; i++)
	{
		descriptor_list[i] = rhs.descriptor_list[i];
	}
}

SdtServiceObject &SdtServiceObject::operator=(SdtServiceObject &rhs)
{
	service_id = rhs.service_id;
	reserved_bits1 = rhs.reserved_bits1;
	EIT_schedule_flag = rhs.EIT_schedule_flag;
	EIT_present_following_flag = rhs.EIT_present_following_flag;
	running_status = rhs.running_status;
	free_CA_mode = rhs.free_CA_mode;
	descriptors_loop_length = rhs.descriptors_loop_length;
	descriptor_list_length = rhs.descriptor_list_length;

	delete additional_content_url_descriptor;
	if(additional_content_url_flag)
	{
		additional_content_url_descriptor = new TsAdditionalContentUrlDescriptorObject();
		*additional_content_url_descriptor = *(rhs.additional_content_url_descriptor);
	}
	
	delete []descriptor_list;
	descriptor_list = new TsDescriptorObject[rhs.descriptor_list_length];
	for(unsigned int i=0; i<rhs.descriptor_list_length; i++)
	{
		descriptor_list[i] = rhs.descriptor_list[i];
	}

	return *this;
}

void SdtServiceObject::print()
{
	printDebugMessage("SDT service object attributes:", INFO);
	printAttribute("service_id", (int)service_id);
	printAttribute("reserved_bits1", (int)reserved_bits1);
	printAttribute("EIT_schedule_flag", (int)EIT_schedule_flag);
	printAttribute("EIT_present_following_flag", (int)EIT_present_following_flag);
	printAttribute("running_status", (int)running_status);
	printAttribute("free_CA_mode", (int)free_CA_mode);
	printAttribute("descriptors_loop_length", (int)descriptors_loop_length);
	printAttribute("descriptor_list_length", (int)descriptor_list_length);
	for(unsigned int i=0; i<descriptor_list_length; i++)
	{
		descriptor_list[i].print();
	}
	if(additional_content_url_descriptor != NULL)
		additional_content_url_descriptor->print();
}

SdtServiceObject::~SdtServiceObject()
{
	delete []descriptor_list;
	delete additional_content_url_descriptor;
}

TsUrlExtension::TsUrlExtension()
{
	URL_extension = NULL;
}

TsUrlExtension::TsUrlExtension(unsigned short int offset, unsigned char bytes[188])
{
	URL_extension = NULL;
	extension_id = bytes[offset++];
	reserved_byte = bytes[offset++];
	URL_extension_length = bytes[offset++];
	URL_extension = new unsigned char[URL_extension_length];
	for(unsigned int i=0; i<URL_extension_length; i++)
	{
		unsigned char byte = bytes[offset++];
		if(byte != '\0')
			URL_extension[i] = byte;
		else
			URL_extension[i] = ' ';
	}
}

TsUrlExtension::TsUrlExtension(TsUrlExtension &rhs)
{
	extension_id  = rhs.extension_id;
	reserved_byte = rhs.reserved_byte;
	URL_extension_length = rhs.URL_extension_length;

	URL_extension = new unsigned char[URL_extension_length];
	for(unsigned char i=0; i<rhs.URL_extension_length; i++)
	{
		URL_extension[i] = rhs.URL_extension[i];
	}
}

TsUrlExtension &TsUrlExtension::operator=(TsUrlExtension &rhs)
{
	extension_id  = rhs.extension_id;
	reserved_byte = rhs.reserved_byte;
	URL_extension_length = rhs.URL_extension_length;

	delete []URL_extension;
	URL_extension = new unsigned char[URL_extension_length];
	for(unsigned char i=0; i<rhs.URL_extension_length; i++)
	{
		URL_extension[i] = rhs.URL_extension[i];
	}
	return *this;
}

void TsUrlExtension::print()
{
	printDebugMessage("TS URL extension attributes:", INFO);
	printAttribute("extension_id", (int)extension_id);
	printAttribute("reserved_byte", (int)reserved_byte);
	printAttribute("URL_extension_length", (int)URL_extension_length);
	printArrayAttribute("URL_extension", URL_extension, URL_extension_length);
}

TsUrlExtension::~TsUrlExtension()
{
	delete []URL_extension;
}

TsAdditionalContentUrlDescriptorObject::TsAdditionalContentUrlDescriptorObject()
{
	URL_base = NULL;
	URL_extensions = NULL;
}

TsAdditionalContentUrlDescriptorObject::TsAdditionalContentUrlDescriptorObject(unsigned short int offset, unsigned char bytes[188])
{
	URL_base = NULL;
	URL_extensions = NULL;
	descriptor_tag = bytes[offset++];
	descriptor_length = bytes[offset++];
	descriptor_tag_extension = bytes[offset++];
	reserved_byte = bytes[offset++];
	for(unsigned int i=2; i<descriptor_length; )
	{
		additional_content_id1 = bytes[offset++] << 8;
		additional_content_id2 = bytes[offset++];
		additional_content_id = additional_content_id1 | additional_content_id2;

		URL_base_length = bytes[offset++];
		URL_base = new unsigned char[URL_base_length];
		for(unsigned int j=0; j<URL_base_length; j++)
		{
			unsigned char byte = bytes[offset++];
			if(byte != '\0')
				URL_base[j] = byte;
			else
				URL_base[j] = ' ';
		}

		URL_extension_count = bytes[offset++];
		URL_extensions = new TsUrlExtension[URL_extension_count];
		TsUrlExtension *current_extension;
		for(unsigned int k=0; k<URL_extension_count; k++)
		{
			current_extension = new TsUrlExtension(offset, bytes);
			URL_extensions[k] = *current_extension;
			delete current_extension;
		}
		break;
	}
}

TsAdditionalContentUrlDescriptorObject::TsAdditionalContentUrlDescriptorObject(TsAdditionalContentUrlDescriptorObject &rhs)
{
	descriptor_tag = rhs.descriptor_tag;
	descriptor_length = rhs.descriptor_length;
	descriptor_tag_extension = rhs.descriptor_tag_extension;
	reserved_byte = rhs.reserved_byte;
	additional_content_id = rhs.additional_content_id;
	URL_base_length  = rhs.URL_base_length;
	URL_extension_count = rhs.URL_extension_count;
	URL_base = new unsigned char[URL_base_length];
	for(unsigned char i=0; i<rhs.URL_base_length; i++)
	{
		URL_base[i] = rhs.URL_base[i];
	}
	URL_extensions = new TsUrlExtension[URL_extension_count];
	for(unsigned char i=0; i<rhs.URL_extension_count; i++)
	{
		URL_extensions[i] = rhs.URL_extensions[i];
	}
}

TsAdditionalContentUrlDescriptorObject &TsAdditionalContentUrlDescriptorObject::operator=(TsAdditionalContentUrlDescriptorObject &rhs)
{
	descriptor_tag = rhs.descriptor_tag;
	descriptor_length = rhs.descriptor_length;
	descriptor_tag_extension = rhs.descriptor_tag_extension;
	reserved_byte = rhs.reserved_byte;
	additional_content_id = rhs.additional_content_id;
	URL_base_length  = rhs.URL_base_length;
	URL_extension_count = rhs.URL_extension_count;
	delete []URL_base;
	URL_base = new unsigned char[URL_base_length];
	for(unsigned char i=0; i<rhs.URL_base_length; i++)
	{
		URL_base[i] = rhs.URL_base[i];
	}
	delete []URL_extensions;
	URL_extensions = new TsUrlExtension[URL_extension_count];
	for(unsigned char i=0; i<rhs.URL_extension_count; i++)
	{
		URL_extensions[i] = rhs.URL_extensions[i];
	}
	return *this;
}

string TsAdditionalContentUrlDescriptorObject::getUrl()
{
	string url = "http://";
	for(unsigned char i=0; i<URL_base_length; i++)
	{
		url += (char)URL_base[i];
	}
	for(unsigned char i=0; i<URL_extension_count; i++)
	{
		for(unsigned char j=0; j<URL_extensions[i].URL_extension_length; j++)
		{
			url += (char)(URL_extensions[i].URL_extension[j]);
		}
	}
	return url;
}

void TsAdditionalContentUrlDescriptorObject::print()
{
	printDebugMessage("TS additional content URL descriptor attributes:", INFO);
	printAttribute("descriptor_tag", (int)descriptor_tag);
	printAttribute("descriptor_length", (int)descriptor_length);
	printAttribute("descriptor_tag_extension", (int)descriptor_tag_extension);
	printAttribute("reserved_byte", (int)reserved_byte);
	printAttribute("additional_content_id", (int)additional_content_id);
	printAttribute("URL_base_length", (int)URL_base_length);
	printAttribute("URL_extension_count", (int)URL_extension_count);
	printArrayAttribute("URL_base", URL_base, URL_base_length);
	for(unsigned int i=0; i<URL_extension_count; i++)
	{
		URL_extensions[i].print();
	}
}

TsAdditionalContentUrlDescriptorObject::~TsAdditionalContentUrlDescriptorObject()
{
	delete []URL_base;
	delete []URL_extensions;
}

TsHeader::TsHeader(){}

TsHeader::TsHeader(unsigned short int offset, unsigned char bytes[188])
{
	sync_byte = bytes[offset++];
	
	transport_error_indicator = (bytes[offset] & 0x80) >> 7;
	payload_unit_start_indicator = (bytes[offset] & 0x40) >> 6;
	transport_priority = (bytes[offset] & 0x20) >> 5;
	
	pid1 = (bytes[offset++] & 0x1F) << 8;
	pid2 = bytes[offset++];
	pid = pid1 | pid2;
	
	transport_scrambling_control = (bytes[offset] & 0xC0) >> 6;
	adaptation_field_control = (adaptation_field_control_type_t)((bytes[offset] & 0x30) >> 4);
	continuity_counter = bytes[offset++] & 0x0F;
}

void TsHeader::check()
{
	if(sync_byte != SYNC_BYTE)
	{
		printDebugMessage("sync byte error", ERROR);
	}
	
	if(transport_error_indicator)
	{
		printDebugMessage("transport error", ERROR);
	}
	
	if(adaptation_field_control == reserved_adaptation_field_control)
	{
		printDebugMessage("reserved adaptation field control " + to_str((long long int)pid), ERROR);
	}
}

void TsHeader::print()
{
	printDebugMessage("TS header attributes:", INFO);
	printAttribute("sync_byte", (int)sync_byte);
	printAttribute("transport_error_indicator", (int)transport_error_indicator);
	printAttribute("payload_unit_start_indicator", (int)payload_unit_start_indicator);
	printAttribute("transport_priority", (int)transport_priority);
	printAttribute("pid", (int)pid);
	printAttribute("transport_scrambling_control", (int)transport_scrambling_control);
	printAttribute("adaptation_field_control", adaptation_field_control_type_names[adaptation_field_control]);
	printAttribute("continuity_counter", (int)continuity_counter);
}

TsHeader::~TsHeader(){}

TsPesHeader::TsPesHeader()
{
	isPesStarted = false;
	hasError = false;
}

TsPesHeader::TsPesHeader(unsigned short int offset, unsigned char bytes[188])
{
	isPesStarted = false;
	hasError = false;
	
	if(offset + PES_HEADER_SIZE < TS_PACKET_SIZE)
	{	
		packet_start_code_prefix1 = bytes[offset++] << 16;
		packet_start_code_prefix2 = bytes[offset++] << 8;
		packet_start_code_prefix3 = bytes[offset++];
		packet_start_code_prefix = packet_start_code_prefix1 | packet_start_code_prefix2 | packet_start_code_prefix3;
		
		if(packet_start_code_prefix == PACKET_START_CODE_PREFIX)
		{
			isPesStarted = true;
			
			stream_id = bytes[offset++];
		
			PES_packet_length1 = bytes[offset++] << 8;
			PES_packet_length2 = bytes[offset++];
			PES_packet_length = PES_packet_length1 | PES_packet_length2;
			
			reserved_bits = (bytes[offset] & 0xC0) >> 6;
			PES_scrambling_control = (bytes[offset] & 0x30) >> 4;
			PES_priority = (bytes[offset] & 0x08) >> 3;
			data_alignment_indicator = (bytes[offset] & 0x04) >> 2;
			copyright = (bytes[offset] & 0x02) >> 1;
			original_or_copy = bytes[offset++] & 0x01;
			pts_dts_flag = (bytes[offset] & 0xC0) >> 6;
			escr_flag = (bytes[offset] & 0x20) >> 5;
			es_rate_flag = (bytes[offset] & 0x10) >> 4;
			DSM_trick_mode_flag = (bytes[offset] & 0x08) >> 3;
			additional_copy_info_flag = (bytes[offset] & 0x04) >> 2;
			previous_PES_crc_flag = (bytes[offset] & 0x02) >> 1;
			PES_extension_flag = bytes[offset++] & 0x01;
			PES_header_data_len = bytes[offset++];
			
			if(pts_dts_flag == pts_only || pts_dts_flag == pts_and_dts)
			{
				pts_data1 = bytes[offset++];
				pts_data2 = bytes[offset++];
				pts_data3 = bytes[offset++];
				pts_data4 = bytes[offset++];
				pts_data5 = bytes[offset++];
				
				PTS1 = (pts_data1 & 0x0E) >> 1;
				PTS_marker_bit1 = pts_data1 & 0x01;
				
				PTS2 = (pts_data2 << 7) | ((pts_data3 & 0xFE) >> 1);
				PTS_marker_bit2 = pts_data3 & 0x01;
				
				PTS3 = (pts_data4 << 7) | ((pts_data5 & 0xFE) >> 1);
				PTS_marker_bit3 = pts_data5 & 0x01;
				
				PTS = (PTS1 << 30) | (PTS2 << 15) | PTS3;
				
				if(write_input_to_file_flag)
				{
					unsigned int pid1 = (bytes[1] & 0x1F) << 8;
					unsigned int pid2 = bytes[2];
					unsigned int pid = pid1 | pid2;
				
					writeDebugMessage(to_str(pid) + "_pts", to_str(PTS), STATUS);
				}

				PTS += (prebuffering_as_seconds * 90000 * pts_factor) / pts_divisor;

				if(pts_dts_flag == pts_and_dts)
				{
					dts_data1 = bytes[offset++];
					dts_data2 = bytes[offset++];
					dts_data3 = bytes[offset++];
					dts_data4 = bytes[offset++];
					dts_data5 = bytes[offset++];
					
					DTS1 = (dts_data1 & 0x0E) >> 1;
					DTS_marker_bit1 = dts_data1 & 0x01;
					
					DTS2 = (dts_data2 << 7) | ((dts_data3 & 0xFE) >> 1);
					DTS_marker_bit2 = dts_data3 & 0x01;
					
					DTS3 = (dts_data4 << 7) | ((dts_data5 & 0xFE) >> 1);
					DTS_marker_bit3 = dts_data5 & 0x01;
					
					DTS = (DTS1 << 30) | (DTS2 << 15) | DTS3;
				}
				
				check();
			}
		}
	}
}

void TsPesHeader::check()
{
	if(pts_dts_flag == forbidden)
	{
		hasError = true;
		printDebugMessage("TS PES HEADER ERROR: forbidden pts_dts_flag", ERROR);
	}
	if(pts_dts_flag == pts_only || pts_dts_flag == pts_and_dts)
	{
		if(!PTS_marker_bit1)
		{
			hasError = true;
			printDebugMessage("TS PES HEADER ERROR: PTS_marker_bit 1 is not set", ERROR);
		}
		if(!PTS_marker_bit2)
		{
			hasError = true;
			printDebugMessage("TS PES HEADER ERROR: PTS_marker_bit 2 is not set", ERROR);
		}
		if(!PTS_marker_bit3)
		{
			hasError = true;
			printDebugMessage("TS PES HEADER ERROR: PTS_marker_bit 3 is not set", ERROR);
		}
		if(pts_dts_flag == pts_and_dts)
		{
			if(!DTS_marker_bit1)
			{
				hasError = true;
				printDebugMessage("TS PES HEADER ERROR: DTS_marker_bit 1 is not set", ERROR);
			}
			if(!DTS_marker_bit2)
			{
				hasError = true;
				printDebugMessage("TS PES HEADER ERROR: DTS_marker_bit 2 is not set", ERROR);
			}
			if(!DTS_marker_bit3)
			{
				hasError = true;
				printDebugMessage("TS PES HEADER ERROR: DTS_marker_bit 3 is not set", ERROR);
			}
		}
	}
}

bool TsPesHeader::hasPTS()
{
	return isPesStarted && (pts_dts_flag == pts_only || pts_dts_flag == pts_and_dts);
}

void TsPesHeader::print()
{
	if(!isPesStarted || hasError)
		return;
	
	printDebugMessage("TS PES header attributes:", INFO);
	printAttribute("packet_start_code_prefix", (int)packet_start_code_prefix);
	printAttribute("stream_id", (int)stream_id);
	printAttribute("PES_packet_length", (int)PES_packet_length);
	printAttribute("reserved_bits", (int)reserved_bits);
	printAttribute("PES_scrambling_control", (int)PES_scrambling_control);
	printAttribute("PES_priority", (int)PES_priority);
	printAttribute("data_alignment_indicator", (int)data_alignment_indicator);
	printAttribute("copyright", (int)copyright);
	printAttribute("original_or_copy", (int)original_or_copy);
	printAttribute("pts_dts_flag", pts_dts_flag_status_names[pts_dts_flag]);
	printAttribute("escr_flag", (int)escr_flag);
	printAttribute("es_rate_flag", (int)es_rate_flag);
	printAttribute("DSM_trick_mode_flag", (int)DSM_trick_mode_flag);
	printAttribute("additional_copy_info_flag", (int)additional_copy_info_flag);
	printAttribute("previous_PES_crc_flag", (int)previous_PES_crc_flag);
	printAttribute("PES_extension_flag", (int)PES_extension_flag);
	printAttribute("PES_header_data_len", (int)PES_header_data_len);
	
	if(pts_dts_flag == pts_only || pts_dts_flag == pts_and_dts)
	{
		printAttribute("PTS", PTS);
		if(pts_dts_flag == pts_and_dts)
		{
			printAttribute("DTS", DTS);
		}
	}
}

TsPesHeader::~TsPesHeader(){}

TsAdaptationField::TsAdaptationField()
{
	extension = NULL;
	private_data_bytes = NULL;
}

TsAdaptationField::TsAdaptationField(unsigned short int offset, unsigned char bytes[188])
{
	extension = NULL;
	private_data_bytes = NULL;
	
	discontinuity_indicator = (bytes[offset] & 0x80) >> 7;
	random_access_indicator = (bytes[offset] & 0x40) >> 6;
	elementary_stream_priority_indicator = (bytes[offset] & 0x20) >> 5;
	PCR_flag = (bytes[offset] & 0x10) >> 4;
	OPCR_flag = (bytes[offset] & 0x08) >> 3;
	splicing_point_flag = (bytes[offset] & 0x04) >> 2;
	transport_private_data_flag = (bytes[offset] & 0x02) >> 1;
	adaptation_field_extension_flag = bytes[offset++] & 0x01;
	
	if(PCR_flag)
	{
		PCR_base1 = bytes[offset++] << 25;
		PCR_base2 = bytes[offset++] << 17;
		PCR_base3 = bytes[offset++] << 9;
		PCR_base4 = bytes[offset++] << 1;
		PCR_base5 = (bytes[offset] & 0x80)>>7;
		
		PCR_base = (PCR_base1 | PCR_base2 | PCR_base3 | PCR_base4 | PCR_base5);
		
		PCR_reserved = (bytes[offset] & 0x7E) >> 1;
		
		PCR_extension1 = (bytes[offset++] & 0x01) << 8;
		PCR_extension2 = bytes[offset++];
		
		PCR_extension = PCR_extension1 | PCR_extension2;
	}
	
	if(OPCR_flag)
	{
		OPCR_base1 = bytes[offset++] << 25;
		OPCR_base2 = bytes[offset++] << 17;
		OPCR_base3 = bytes[offset++] << 9;
		OPCR_base4 = bytes[offset++] << 1;
		OPCR_base5 = bytes[offset] & 0x80;
		
		OPCR_base = (OPCR_base1 | OPCR_base2 | OPCR_base3 | OPCR_base4 | OPCR_base5) >> 7;
		
		OPCR_reserved = (bytes[offset] & 0x7E) >> 1;
		
		OPCR_extension1 = (bytes[offset++] & 0x01) << 8;
		OPCR_extension2 = bytes[offset++];
		
		OPCR_extension = OPCR_extension1 | OPCR_extension2;
	}
	
	if(splicing_point_flag)
	{
		splice_countdown = bytes[offset++];
	}
	
	if(transport_private_data_flag)
	{
		transport_private_data_length = bytes[offset++];
		private_data_bytes = new unsigned char[transport_private_data_length];
		for(unsigned char i=0; i<transport_private_data_length; i++)
		{
			private_data_bytes[i] = bytes[offset++];
		}
	}
	
	if(adaptation_field_extension_flag)
	{
		extension = new TsAdaptationFieldExtension(offset, bytes);
	}
}

TsAdaptationField::TsAdaptationField(TsAdaptationField &rhs)
{
	discontinuity_indicator = rhs.discontinuity_indicator;
	random_access_indicator = rhs.random_access_indicator;
	elementary_stream_priority_indicator = rhs.elementary_stream_priority_indicator;
	PCR_flag = rhs.PCR_flag;
	OPCR_flag = rhs.OPCR_flag;
	splicing_point_flag = rhs.splicing_point_flag;
	transport_private_data_flag = rhs.transport_private_data_flag;
	adaptation_field_extension_flag = rhs.adaptation_field_extension_flag;
	
	if(rhs.PCR_flag)
	{
		PCR_base = rhs.PCR_base;
		PCR_reserved = rhs.PCR_reserved;
		PCR_extension = rhs.PCR_extension;
	}
	
	if(rhs.OPCR_flag)
	{
		OPCR_base = rhs.OPCR_base;
		OPCR_reserved = rhs.OPCR_reserved;
		OPCR_extension = rhs.OPCR_extension;
	}
	
	if(rhs.splicing_point_flag)
	{
		splice_countdown = rhs.splice_countdown;
	}
	
	if(rhs.transport_private_data_flag)
	{
		transport_private_data_length = rhs.transport_private_data_length;
		private_data_bytes = new unsigned char[transport_private_data_length];
		for(unsigned char i=0; i<rhs.transport_private_data_length; i++)
		{
			private_data_bytes[i] = rhs.private_data_bytes[i];
		}
	}
	
	if(rhs.adaptation_field_extension_flag)
	{
		*extension = *(rhs.extension);
	}
}

TsAdaptationField &TsAdaptationField::operator=(TsAdaptationField &rhs)
{
	discontinuity_indicator = rhs.discontinuity_indicator;
	random_access_indicator = rhs.random_access_indicator;
	elementary_stream_priority_indicator = rhs.elementary_stream_priority_indicator;
	PCR_flag = rhs.PCR_flag;
	OPCR_flag = rhs.OPCR_flag;
	splicing_point_flag = rhs.splicing_point_flag;
	transport_private_data_flag = rhs.transport_private_data_flag;
	adaptation_field_extension_flag = rhs.adaptation_field_extension_flag;
	
	if(rhs.PCR_flag)
	{
		PCR_base = rhs.PCR_base;
		PCR_reserved = rhs.PCR_reserved;
		PCR_extension = rhs.PCR_extension;
	}
	
	if(rhs.OPCR_flag)
	{
		OPCR_base = rhs.OPCR_base;
		OPCR_reserved = rhs.OPCR_reserved;
		OPCR_extension = rhs.OPCR_extension;
	}
	
	if(rhs.splicing_point_flag)
	{
		splice_countdown = rhs.splice_countdown;
	}
	
	if(rhs.transport_private_data_flag)
	{
		transport_private_data_length = rhs.transport_private_data_length;
		delete []private_data_bytes;
		private_data_bytes = new unsigned char[transport_private_data_length];
		for(unsigned char i=0; i<rhs.transport_private_data_length; i++)
		{
			private_data_bytes[i] = rhs.private_data_bytes[i];
		}
	}
	
	if(rhs.adaptation_field_extension_flag)
	{
		delete extension;
		*extension = *(rhs.extension);
	}
	
	return *this;
}

void TsAdaptationField::check(){}

void TsAdaptationField::print()
{
	printDebugMessage("TS adaptation field attributes:", INFO);
	printAttribute("discontinuity_indicator", (int)discontinuity_indicator);
	printAttribute("random_access_indicator", (int)random_access_indicator);
	printAttribute("elementary_stream_priority_indicator", (int)elementary_stream_priority_indicator);
	printAttribute("PCR_flag", (int)PCR_flag);
	printAttribute("OPCR_flag", (int)OPCR_flag);
	printAttribute("splicing_point_flag", (int)splicing_point_flag);
	printAttribute("transport_private_data_flag", (int)transport_private_data_flag);
	printAttribute("adaptation_field_extension_flag", (int)adaptation_field_extension_flag);
	
	if(PCR_flag)
	{
		printAttribute("PCR_base", (int)PCR_base);
		printAttribute("PCR_reserved", (int)PCR_reserved);
		printAttribute("PCR_extension", (int)PCR_extension);
	}
	
	if(OPCR_flag)
	{
		printAttribute("OPCR_base", (int)OPCR_base);
		printAttribute("OPCR_reserved", (int)OPCR_reserved);
		printAttribute("OPCR_extension", (int)OPCR_extension);
	}
	
	if(transport_private_data_flag)
	{
		printAttribute("transport_private_data_length", (int)transport_private_data_length);
		printArrayAttribute("private_data_byte", private_data_bytes, transport_private_data_length);
	}
	
	if(adaptation_field_extension_flag)
		extension->print();
}

TsAdaptationField::~TsAdaptationField()
{
	delete []private_data_bytes;
	delete extension;
}

TsAdaptationFieldExtension::TsAdaptationFieldExtension(){}

TsAdaptationFieldExtension::TsAdaptationFieldExtension(unsigned short int offset, unsigned char bytes[188])
{
	adaptation_field_extension_len = bytes[offset++];
	ltw_flag = (bytes[offset] & 0x80) >> 7;
	piecewise_rate_flag = (bytes[offset] & 0x40) >> 6;
	seamless_splice_flag = (bytes[offset] & 0x20) >> 5;
	extension_reserved = bytes[offset++] & 0x10;
	
	if(ltw_flag)
	{
		ltw_valid_flag = (bytes[offset] & 0x80) >> 7;
		
		ltw_offset1 = (bytes[offset++] & 0x7F) << 8;
		ltw_offset2 = bytes[offset++];
		
		ltw_offset = ltw_offset1 | ltw_offset2;
	}
	
	if(piecewise_rate_flag)
	{
		piecewise_rate_reserved = (bytes[offset] & 0xC0) >> 6;
		
		piecewise_rate1 = (bytes[offset++] & 0x3F) << 16;
		piecewise_rate2 = bytes[offset++] << 8;
		piecewise_rate3 = bytes[offset++];
		
		piecewise_rate = piecewise_rate1 | piecewise_rate2 | piecewise_rate3;
	}
	
	if(seamless_splice_flag)
	{
		data0 = bytes[offset++];
		data1 = bytes[offset++];
		data2 = bytes[offset++];
		data3 = bytes[offset++];
		data4 = bytes[offset++];
		
		splice_type = (data0 & 0xF0) >> 4;
		
		DTS_next_AU_1 = (data0 & 0x0E) >> 1;
		DTS_next_AU_marker1 = data0 & 0x01;
		DTS_next_AU_2 = (data1 << 7) | ((data2 & 0xFE) >> 1);
		DTS_next_AU_marker2 = data0 & 0x01;
		DTS_next_AU_3 = (data3 << 7) | ((data4 & 0xFE) >> 1);
		DTS_next_AU_marker3 = data0 & 0x01;
		DTS_next_AU = (DTS_next_AU_1 << 30) | (DTS_next_AU_2 << 15) | DTS_next_AU_marker3;
	}
}

void TsAdaptationFieldExtension::check()
{
	if(!DTS_next_AU_marker1)
	{
		printDebugMessage("DTS next AU marker bit 1 is not set", ERROR);
	}
	
	if(!DTS_next_AU_marker2)
	{
		printDebugMessage("DTS next AU marker bit 2 is not set", ERROR);
	}
	
	if(!DTS_next_AU_marker3)
	{
		printDebugMessage("DTS next AU marker bit 3 is not set", ERROR);
	}
}

void TsAdaptationFieldExtension::print()
{
	printDebugMessage("TS adaptation field extension attributes:", INFO);
	printAttribute("adaptation_field_extension_len", (int)adaptation_field_extension_len);
	printAttribute("ltw_flag", (int)ltw_flag);
	printAttribute("piecewise_rate_flag", (int)piecewise_rate_flag);
	printAttribute("seamless_splice_flag", (int)seamless_splice_flag);
	printAttribute("extension_reserved", (int)extension_reserved);
	
	if(ltw_flag)
	{
		printAttribute("ltw_valid_flag", (int)ltw_valid_flag);
		printAttribute("ltw_offset", (int)ltw_offset);
	}
	
	if(piecewise_rate_flag)
	{
		printAttribute("piecewise_rate_reserved", (int)piecewise_rate_reserved);
		printAttribute("piecewise_rate", (int)piecewise_rate);
	}
	
	if(seamless_splice_flag)
	{
		printAttribute("splice_type", (int)splice_type);
		printAttribute("DTS_next_AU", (int)DTS_next_AU);
	}
}

TsAdaptationFieldExtension::~TsAdaptationFieldExtension(){}

TsPacket::TsPacket()
{
	bytes = NULL;
	header = NULL;
	adaptation_field = NULL;
	packet_payload = NULL;
	packet_status = NO_ERROR;
}

TsPacket::TsPacket(unsigned char *_bytes)
{
	offset = 0;
	bytes = new unsigned char[TS_PACKET_SIZE];
	memcpy(bytes, _bytes, TS_PACKET_SIZE);
	header = new TsHeader(offset, bytes);
	offset += TS_HEADER_SIZE;
	packet_status = NO_ERROR;
	setPacketType();
}

TsPacket::TsPacket(TsPacket &rhs)
{
	offset = 0;
	bytes = new unsigned char[TS_PACKET_SIZE];
	memcpy(bytes, rhs.bytes, TS_PACKET_SIZE);
	header = new TsHeader(offset, bytes);
	offset += TS_HEADER_SIZE;
	packet_type = rhs.packet_type;
	packet_status = rhs.packet_status;
	parse();
}

TsPacket &TsPacket::operator=(TsPacket &rhs)
{
	delete []bytes;
	delete header;
	delete adaptation_field;
	delete packet_payload;
	
	offset = 0;
	bytes = new unsigned char[TS_PACKET_SIZE];
	memcpy(bytes, rhs.bytes, TS_PACKET_SIZE);
	header = new TsHeader(offset, bytes);
	offset += TS_HEADER_SIZE;
	packet_type = rhs.packet_type;
	packet_status = rhs.packet_status;
	parse();

	return *this;
}

bool TsPacket::operator==(const TsPacket &rhs)
{
	for(unsigned short int i=0; i<TS_PACKET_SIZE; i++)
	{
		if(i < 4)
			continue; //header must already be the same
		if(bytes[i] != rhs.bytes[i])
			return false;
	}
	return true;
}

bool TsPacket::operator!=(const TsPacket &rhs)
{
	return !operator==(rhs);
}

void TsPacket::setPacketType()
{
	switch(header->pid)
	{
		case TS_PAT_PID:
			packet_type = TS_PAT_PACKET;
			break;
		case TS_CAT_PID:
			packet_type = TS_CAT_PACKET;
			break;
		case TS_TSDT_PID:
			packet_type = TS_TSDT_PACKET;
			break;
		case TS_NIT_PID:
			packet_type = TS_NIT_PACKET;
			break;
		case TS_SDT_PID:
			packet_type = TS_SDT_PACKET;
			break;
		case TS_EIT_PID:
			packet_type = TS_EIT_PACKET;
			break;
		case TS_NULL_PID:
			packet_type = TS_NULL_PACKET;
			break;
		default:
			packet_type = TS_UNKNOWN_PACKET;
			break;
	}
}

void TsPacket::setPacketType(ts_packet_type_t type)
{
	packet_type = type;
}

void TsPacket::parse()
{
	adaptation_field = NULL;
	packet_payload = NULL;
	
	if(!isParseable())
		return;
	
	adaptation_field_length = bytes[offset++];
	adaptation_field_length = header->adaptation_field_control == no_adaptation_field_payload_only ? 0 : adaptation_field_length;
	if(adaptation_field_length > 0)
		adaptation_field = new TsAdaptationField(offset, bytes);
	else
		adaptation_field = NULL;
	offset += adaptation_field_length;
	
	if(header->adaptation_field_control == no_payload_adaptation_field_only)
		return;
	
	switch(packet_type)
	{
		case TS_PAT_PACKET:
			packet_payload = new TsPatPacket(offset, bytes);
			if(packet_payload->table_id != 0x0)
			{
				delete packet_payload;
				packet_type = TS_UNKNOWN_PACKET;
				packet_payload = NULL;
			}
			break;
		case TS_CAT_PACKET:
			break;
		case TS_TSDT_PACKET:
			break;
		case TS_NIT_PACKET:
			packet_payload = new TsNitPacket(offset, bytes);
			if(packet_payload->table_id != 0x40 && packet_payload->table_id != 0x41)
			{
				delete packet_payload;
				packet_type = TS_UNKNOWN_PACKET;
				packet_payload = NULL;
			}
			break;
		case TS_SDT_PACKET:
			packet_payload = new TsSdtPacket(offset, bytes);
			if(packet_payload->table_id != 0x42 && packet_payload->table_id != 0x46)
			{
				delete packet_payload;
				packet_type = TS_BAT_PACKET;
				packet_payload = new TsBatPacket(offset, bytes);
				if(packet_payload->table_id != 0x42 && packet_payload->table_id != 0x4A)
				{
					packet_type = TS_UNKNOWN_PACKET;
					packet_payload = NULL;
				}
			}
			break;
		case TS_EIT_PACKET:
            packet_payload = new TsEitPacket(offset, bytes);
            if(packet_payload->table_id < 0x4e || packet_payload->table_id > 0x6f)
            {
				delete packet_payload;
				packet_type = TS_UNKNOWN_PACKET;
				packet_payload = NULL;
			}
            break;
		case TS_PMT_PACKET:
			packet_payload = new TsPmtPacket(offset, bytes);
			if(packet_payload->table_id != 0x02)
			{
				delete packet_payload;
				packet_type = TS_UNKNOWN_PACKET;
				packet_payload = NULL;
			}
			break;
		case TS_PES_VIDEO_PACKET:
		case TS_PES_AUDIO_PACKET:
		case TS_PES_PRIVATE_DATA_PACKET:
			if(header->adaptation_field_control == no_adaptation_field_payload_only)
				offset--;	//adaptation_field_length does not exist because the packet is a PES (is this a problem of the TS packet structure?)
			packet_payload = new TsPesPacket(offset, bytes);
			break;
		case TS_PCR_PACKET:
		case TS_NULL_PACKET:
		case TS_UNKNOWN_PACKET:
		default:
			packet_payload = NULL;
			break;
	}
}

void TsPacket::check()
{
	packet_status = getPacketStatus();
}

ts_packet_status_t TsPacket::getPacketStatus()
{
	if(header->transport_error_indicator)
		return TRANSPORT_ERROR;
	if(header->adaptation_field_control == reserved_adaptation_field_control)
		return ADAPTATION_FIELD_ERROR;
	if(header->pid == TS_NULL_PID)
		return NULL_PACKET_STATUS;
	if(packet_type == TS_UNKNOWN_PACKET)
		return UNKNOWN_PACKET_STATUS;
	return NO_ERROR;
}

bool TsPacket::isParseable()
{
	if(packet_status == NO_ERROR)
		return true;
	return false;
}

bool TsPacket::hasPCR()
{
	if(isParseable() &&
		((header->adaptation_field_control == no_payload_adaptation_field_only) || (header->adaptation_field_control == adaptation_field_and_payload)) &&
		(adaptation_field_length > 0) &&
		adaptation_field->PCR_flag)
		return true;
	return false;
}

unsigned long long TsPacket::getPCRbase()
{
	return (hasPCR() ? adaptation_field->PCR_base : 0);
}

unsigned short int TsPacket::getPCRextension()
{
	return (hasPCR() ? adaptation_field->PCR_extension : 0);
}

void TsPacket::print()
{
	if(!isParseable())
		return;
	
	printDebugMessage("****************************", INFO);
	printDebugMessage("TS packet attributes:", INFO);
	printAttribute("packet_type", ts_packet_type_names[packet_type]);
	printAttribute("packet_status", ts_packet_status_names[packet_status]);
	printAttribute("adaptation_field_length", (int)adaptation_field_length);
	header->print();
	if(isParseable())
	{
		if(adaptation_field_length > 0)
			adaptation_field->print();
		if(header->adaptation_field_control != no_payload_adaptation_field_only && packet_payload != NULL)
		{
			packet_payload->print();
		}
	}
	printDebugMessage("****************************", INFO);
}

void TsPacket::printBytes()
{
	printDebugMessage("-------------------------------------", INFO);
	for(int i=0; i<TS_PACKET_SIZE; i++)
	{
		cout << (int)bytes[i] << " ";
	}
	cout << endl;
	printDebugMessage("-------------------------------------", INFO);
}

TsPacket::~TsPacket()
{
	delete []bytes;
	delete header;
	delete adaptation_field;
	delete packet_payload;
}

TsPayloadBase::TsPayloadBase()
{
	hasError = false;
}

TsPayloadBase::TsPayloadBase(unsigned short int offset, unsigned char bytes[188])
{
	hasError = false;
}

string TsPayloadBase::getUrl()
{
	for(unsigned int i=0; i<service_list_length; i++)
	{
		if(service_list[i].additional_content_url_descriptor != NULL)
			return service_list[i].additional_content_url_descriptor->getUrl();
	}
	return "";
}

bool TsPayloadBase::isStartOfFrame()
{
	return (header != NULL) && header->hasPTS();
}

bool TsPayloadBase::isNewFrame(unsigned int PTS)
{
	return isStartOfFrame() && (header->PTS != PTS);
}

TsPayloadBase::~TsPayloadBase(){}

TsPatPacket::TsPatPacket(){}

TsPatPacket::TsPatPacket(unsigned short int offset, unsigned char bytes[188])
{
	/* default */
	program_list_length = 0;
	program_list = new PatProgramObject[33];
	nit_pid = TS_NIT_PID;
	/* default */
	
	table_id = bytes[offset++];
	if(table_id != 0x0)
	{
		//printDebugMessage("table id " + to_str(table_id) + " not handled for pid " + to_str(TS_PAT_PID), WARNING);
		return;
	}
	
	section_syntax_indicator = (bytes[offset] & 0x80) >> 7;
	constant_zero_bit = (bytes[offset] & 0x40) >> 6;
	reserved_bits1 = (bytes[offset] & 0x30) >> 4;
	
	section_length1 = (bytes[offset++] & 0x0F) << 8;
	section_length2 = bytes[offset++];
	section_length = section_length1 | section_length2;
	
	transport_stream_id1 = bytes[offset++] << 8;
	transport_stream_id2 = bytes[offset++];
	transport_stream_id = transport_stream_id1 | transport_stream_id2;
	
	reserved_bits2 = (bytes[offset] & 0xC0) >> 6;
	version_number = (bytes[offset] & 0x3E) >> 1;
	current_next_indicator = bytes[offset++] & 0x01;
	section_number = bytes[offset++];
	last_section_number = bytes[offset++];
	
	//N LOOP for program pmt pids
	PatProgramObject *current_program;
	while(offset < section_length + 3)
	{
		if(offset >= TS_PACKET_SIZE - TS_CRC32_SIZE)
			break;
		current_program = new PatProgramObject(offset, bytes);
		offset += 4;
		
		if(current_program->program_no == TS_NIT_PROGRAM_NO && current_program->pmt_pid != nit_pid)
		{
			nit_pid = current_program->pmt_pid;
			printDebugMessage("TS NIT PID update", STATUS);
		}
		else
		{
			program_list[program_list_length++] = *current_program;
		}
		delete current_program;
	}
	
	crc321 = bytes[offset++] << 24;
	crc322 = bytes[offset++] << 16;
	crc323 = bytes[offset++] << 8;
	crc324 = bytes[offset++];
	crc32 = crc321 | crc322 | crc323 | crc324;
}

TsPatPacket::TsPatPacket(TsPatPacket &rhs)
{
	table_id = rhs.table_id;
	section_syntax_indicator = rhs.section_syntax_indicator;
	constant_zero_bit = rhs.constant_zero_bit;
	reserved_bits1 = rhs.reserved_bits1;
	section_length = rhs.section_length;
	transport_stream_id = rhs.transport_stream_id;
	reserved_bits2 = rhs.reserved_bits2;
	version_number = rhs.version_number;
	current_next_indicator = rhs.current_next_indicator;
	section_number = rhs.section_number;
	last_section_number = rhs.last_section_number;
	crc32 = rhs.crc32;
	nit_pid = rhs.nit_pid;
	program_list_length = rhs.program_list_length;
	program_list = new PatProgramObject[33];
	for(unsigned int i=0; i<rhs.program_list_length; i++)
	{
		program_list[i] = rhs.program_list[i];
	}
}

TsPatPacket &TsPatPacket::operator=(TsPatPacket &rhs)
{
	table_id = rhs.table_id;
	section_syntax_indicator = rhs.section_syntax_indicator;
	constant_zero_bit = rhs.constant_zero_bit;
	reserved_bits1 = rhs.reserved_bits1;
	section_length = rhs.section_length;
	transport_stream_id = rhs.transport_stream_id;
	reserved_bits2 = rhs.reserved_bits2;
	version_number = rhs.version_number;
	current_next_indicator = rhs.current_next_indicator;
	section_number = rhs.section_number;
	last_section_number = rhs.last_section_number;
	crc32 = rhs.crc32;
	nit_pid = rhs.nit_pid;
	program_list_length = rhs.program_list_length;
	delete []program_list;
	program_list = new PatProgramObject[33];
	for(unsigned int i=0; i<rhs.program_list_length; i++)
	{
		program_list[i] = rhs.program_list[i];
	}

	return *this;
}

void TsPatPacket::check(){}

void TsPatPacket::print()
{
	if(table_id != 0x0)
		return;
	printDebugMessage("TS PAT packet attributes:", INFO);
	printAttribute("table_id", (int)table_id);
	printAttribute("section_syntax_indicator", (int)section_syntax_indicator);
	printAttribute("constant_zero_bit", (int)constant_zero_bit);
	printAttribute("reserved_bits1", (int)reserved_bits1);
	printAttribute("section_length", (int)section_length);
	printAttribute("transport_stream_id", (int)transport_stream_id);
	printAttribute("reserved_bits2", (int)reserved_bits2);
	printAttribute("version_number", (int)version_number);
	printAttribute("current_next_indicator", (int)current_next_indicator);
	printAttribute("section_number", (int)section_number);
	printAttribute("last_section_number", (int)last_section_number);
	printAttribute("nit_pid", (int)nit_pid);
	printAttribute("crc32", (int)crc32);
	printAttribute("program_list_length", (int)program_list_length);
	for(unsigned int i=0; i<program_list_length; i++)
	{
		program_list[i].print();
	}
}

TsPatPacket::~TsPatPacket()
{
	delete []program_list;
}

TsEitPacket::TsEitPacket(){}

TsEitPacket::TsEitPacket(unsigned short int offset, unsigned char bytes[188])
{
    /* default */
    event_list_length = 0;
    event_list = new EitEventObject[33];
    /* default */

    table_id = bytes[offset++];
    if(table_id < 0x4e || table_id > 0x6f)
    {
		//printDebugMessage("table id " + to_str(table_id) + " not handled for pid " + to_str(TS_EIT_PID), WARNING);
		return;
	}
    
    section_syntax_indicator = (bytes[offset] & 0x80) >> 7;
    constant_zero_bit = (bytes[offset] & 0x40) >> 6;
    reserved_bits1 = (bytes[offset] & 0x30) >> 4;

    section_length1 = (bytes[offset++] & 0x0F) << 8;
    section_length2 = bytes[offset++];
    section_length = section_length1 | section_length2;

    service_id1 = bytes[offset++] << 8;
    service_id2 = bytes[offset++];
    service_id = service_id1 | service_id2;

    reserved_bits2 = (bytes[offset] & 0xC0) >> 6;
    version_number = (bytes[offset] & 0x3E) >> 1;
    current_next_indicator = bytes[offset++] & 0x01;
    section_number = bytes[offset++];
    last_section_number = bytes[offset++];

    transport_stream_id1 = (bytes[offset++] & 0x0F) << 8;
    transport_stream_id2= bytes[offset++];
    transport_stream_id = transport_stream_id1 | transport_stream_id2;

    original_network_id1 = bytes[offset++] << 8;
    original_network_id2 = bytes[offset++];
    original_network_id = original_network_id1 | original_network_id2;

    segment_last_section_number = bytes[offset++];
    last_table_id = bytes[offset++];

    //N LOOP for events
    EitEventObject *current_event;
    while(offset < section_length + 3)
    {
		if(offset >= TS_PACKET_SIZE - TS_CRC32_SIZE)
			break;
        current_event = new EitEventObject(offset, bytes);
        offset += current_event->getTotalLength();
        event_list[event_list_length++] = *current_event;
        delete current_event;
    }

    crc321 = bytes[offset++] << 24;
    crc322 = bytes[offset++] << 16;
    crc323 = bytes[offset++] << 8;
    crc324 = bytes[offset++];
    crc32 = crc321 | crc322 | crc323 | crc324;
}

TsEitPacket::TsEitPacket(TsEitPacket &rhs)
{
    table_id = rhs.table_id;
    section_syntax_indicator = rhs.section_syntax_indicator;
    constant_zero_bit = rhs.constant_zero_bit;
    reserved_bits1 = rhs.reserved_bits1;
    section_length = rhs.section_length;
    service_id = rhs.service_id;
    reserved_bits2 = rhs.reserved_bits2;
    version_number = rhs.version_number;
    current_next_indicator = rhs.current_next_indicator;
    section_number = rhs.section_number;
    last_section_number = rhs.last_section_number;
    transport_stream_id = rhs.transport_stream_id;
    original_network_id = rhs.original_network_id;
    segment_last_section_number = rhs.segment_last_section_number;
    last_table_id = rhs.last_table_id;
    crc32 = rhs.crc32;
    event_list_length = rhs.event_list_length;
    event_list = new EitEventObject[33];
    for(unsigned int i=0; i<rhs.event_list_length; i++)
    {
        event_list[i] = rhs.event_list[i];
    }
}

TsEitPacket &TsEitPacket::operator=(TsEitPacket &rhs)
{
    table_id = rhs.table_id;
    section_syntax_indicator = rhs.section_syntax_indicator;
    constant_zero_bit = rhs.constant_zero_bit;
    reserved_bits1 = rhs.reserved_bits1;
    section_length = rhs.section_length;
    service_id = rhs.service_id;
    reserved_bits2 = rhs.reserved_bits2;
    version_number = rhs.version_number;
    current_next_indicator = rhs.current_next_indicator;
    section_number = rhs.section_number;
    last_section_number = rhs.last_section_number;
    transport_stream_id = rhs.transport_stream_id;
    original_network_id = rhs.original_network_id;
    segment_last_section_number = rhs.segment_last_section_number;
    last_table_id = rhs.last_table_id;
    crc32 = rhs.crc32;
    event_list_length = rhs.event_list_length;
    delete []event_list;
    event_list = new EitEventObject[33];
    for(unsigned int i=0; i<rhs.event_list_length; i++)
    {
        event_list[i] = rhs.event_list[i];
    }

    return *this;
}

void TsEitPacket::check(){}

void TsEitPacket::print()
{
	if(table_id < 0x4e || table_id > 0x6f)
		return;
	printDebugMessage("TS EIT packet attributes:", INFO);
	printAttribute("table_id", (int)table_id);
	printAttribute("section_syntax_indicator", (int)section_syntax_indicator);
	printAttribute("constant_zero_bit", (int)constant_zero_bit);
	printAttribute("reserved_bits1", (int)reserved_bits1);
	printAttribute("section_length", (int)section_length);
	printAttribute("service_id", (int)service_id);
	printAttribute("reserved_bits2", (int)reserved_bits2);
	printAttribute("version_number", (int)version_number);
	printAttribute("current_next_indicator", (int)current_next_indicator);
	printAttribute("section_number", (int)section_number);
	printAttribute("last_section_number", (int)last_section_number);
	printAttribute("transport_stream_id", (int)transport_stream_id);
	printAttribute("original_network_id", (int)original_network_id);
	printAttribute("segment_last_section_number", (int)segment_last_section_number);
	printAttribute("original_network_id", (int)original_network_id);
	printAttribute("last_table_id", (int)last_table_id);
	printAttribute("crc32", (int)crc32);
	printAttribute("event_list_length", (int)event_list_length);
	for(unsigned int i=0; i<event_list_length; i++)
	{
		event_list[i].print();
	}
}

TsEitPacket::~TsEitPacket()
{
    delete []event_list;
}

TsBatPacket::TsBatPacket(){}

TsBatPacket::TsBatPacket(unsigned short int offset, unsigned char bytes[188])
{
    /* default */
	descriptor_list_length = 0;
	descriptor_list = new TsDescriptorObject[30];
	
    transport_stream_list_length = 0;
    transport_stream_list = new BatTransportStreamObject[33];
    /* default */

    table_id = bytes[offset++];
    if(table_id != 0x4A)
    {
		//printDebugMessage("table id " + to_str(table_id) + " not handled for pid " + to_str(TS_BAT_PID), WARNING);
		return;
	}
    
    section_syntax_indicator = (bytes[offset] & 0x80) >> 7;
    constant_zero_bit = (bytes[offset] & 0x40) >> 6;
    reserved_bits1 = (bytes[offset] & 0x30) >> 4;

    section_length1 = (bytes[offset++] & 0x0F) << 8;
    section_length2 = bytes[offset++];
    section_length = section_length1 | section_length2;

    bouquet_id1 = bytes[offset++] << 8;
    bouquet_id2 = bytes[offset++];
    bouquet_id = bouquet_id1 | bouquet_id2;

    reserved_bits2 = (bytes[offset] & 0xC0) >> 6;
    version_number = (bytes[offset] & 0x3E) >> 1;
    current_next_indicator = bytes[offset++] & 0x01;
    section_number = bytes[offset++];
    last_section_number = bytes[offset++];
    
    reserved_bits3 = (bytes[offset] & 0xF0) >> 4;
    
    descriptors_loop_length1 = (bytes[offset++] & 0x0F) << 8;
    descriptors_loop_length2 = bytes[offset++];
    descriptors_loop_length = descriptors_loop_length1 | descriptors_loop_length2;
    
    //N LOOP for bouquet descriptors
    unsigned char info_cnt = 0;
    TsDescriptorObject *current_descriptor;
    while(info_cnt < descriptors_loop_length)
    {
		if(offset >= TS_PACKET_SIZE - TS_CRC32_SIZE)
			break;
        current_descriptor = new TsDescriptorObject(offset, bytes);
        descriptor_list[descriptor_list_length++] = *current_descriptor;
        info_cnt += 2 + current_descriptor->descriptor_length;
        offset += 2 + current_descriptor->descriptor_length;
        delete current_descriptor;
    }
    
    reserved_bits4 = (bytes[offset] & 0xF0) >> 4;
    
    transport_stream_loop_length1 = (bytes[offset++] & 0x0F) << 8;
    transport_stream_loop_length2 = bytes[offset++];
    transport_stream_loop_length = transport_stream_loop_length1 | transport_stream_loop_length2;

    //N LOOP for transport streams
    BatTransportStreamObject *current_transport_stream;
    while(offset < section_length + 3)
    {
		if(offset >= TS_PACKET_SIZE - TS_CRC32_SIZE)
			break;
        current_transport_stream = new BatTransportStreamObject(offset, bytes);
        offset += current_transport_stream->getTotalLength();
        transport_stream_list[transport_stream_list_length++] = *current_transport_stream;
        delete current_transport_stream;
    }

    crc321 = bytes[offset++] << 24;
    crc322 = bytes[offset++] << 16;
    crc323 = bytes[offset++] << 8;
    crc324 = bytes[offset++];
    crc32 = crc321 | crc322 | crc323 | crc324;
}

TsBatPacket::TsBatPacket(TsBatPacket &rhs)
{
    table_id = rhs.table_id;
    section_syntax_indicator = rhs.section_syntax_indicator;
    constant_zero_bit = rhs.constant_zero_bit;
    reserved_bits1 = rhs.reserved_bits1;
    section_length = rhs.section_length;
    bouquet_id = rhs.bouquet_id;
    reserved_bits2 = rhs.reserved_bits2;
    version_number = rhs.version_number;
    current_next_indicator = rhs.current_next_indicator;
    section_number = rhs.section_number;
    last_section_number = rhs.last_section_number;
    reserved_bits3 = rhs.reserved_bits3;
    descriptors_loop_length = rhs.descriptors_loop_length;
    reserved_bits4 = rhs.reserved_bits4;
    transport_stream_loop_length = rhs.transport_stream_loop_length;
    crc32 = rhs.crc32;
    
    descriptor_list_length = rhs.descriptor_list_length;
    descriptor_list = new TsDescriptorObject[30];
    for(unsigned int i=0; i<rhs.descriptor_list_length; i++)
    {
		descriptor_list[i] = rhs.descriptor_list[i];
    }
    
    transport_stream_list_length = rhs.transport_stream_list_length;
    transport_stream_list = new BatTransportStreamObject[33];
    for(unsigned int i=0; i<rhs.transport_stream_list_length; i++)
    {
        transport_stream_list[i] = rhs.transport_stream_list[i];
    }
}

TsBatPacket &TsBatPacket::operator=(TsBatPacket &rhs)
{
    table_id = rhs.table_id;
    section_syntax_indicator = rhs.section_syntax_indicator;
    constant_zero_bit = rhs.constant_zero_bit;
    reserved_bits1 = rhs.reserved_bits1;
    section_length = rhs.section_length;
    bouquet_id = rhs.bouquet_id;
    reserved_bits2 = rhs.reserved_bits2;
    version_number = rhs.version_number;
    current_next_indicator = rhs.current_next_indicator;
    section_number = rhs.section_number;
    last_section_number = rhs.last_section_number;
    reserved_bits3 = rhs.reserved_bits3;
    descriptors_loop_length = rhs.descriptors_loop_length;
    reserved_bits4 = rhs.reserved_bits4;
    transport_stream_loop_length = rhs.transport_stream_loop_length;
    crc32 = rhs.crc32;
    
    delete []descriptor_list;
    descriptor_list_length = rhs.descriptor_list_length;
    descriptor_list = new TsDescriptorObject[30];
    for(unsigned int i=0; i<rhs.descriptor_list_length; i++)
    {
		descriptor_list[i] = rhs.descriptor_list[i];
    }
    
    delete []transport_stream_list;
    transport_stream_list_length = rhs.transport_stream_list_length;
    transport_stream_list = new BatTransportStreamObject[33];
    for(unsigned int i=0; i<rhs.transport_stream_list_length; i++)
    {
        transport_stream_list[i] = rhs.transport_stream_list[i];
    }

    return *this;
}

void TsBatPacket::check(){}

void TsBatPacket::print()
{
	if(table_id != 0x4A)
		return;
	printDebugMessage("TS BAT packet attributes:", INFO);
	printAttribute("table_id", (int)table_id);
	printAttribute("section_syntax_indicator", (int)section_syntax_indicator);
	printAttribute("constant_zero_bit", (int)constant_zero_bit);
	printAttribute("reserved_bits1", (int)reserved_bits1);
	printAttribute("section_length", (int)section_length);
	printAttribute("bouquet_id", (int)bouquet_id);
	printAttribute("reserved_bits2", (int)reserved_bits2);
	printAttribute("version_number", (int)version_number);
	printAttribute("current_next_indicator", (int)current_next_indicator);
	printAttribute("section_number", (int)section_number);
	printAttribute("last_section_number", (int)last_section_number);
	printAttribute("reserved_bits3", (int)reserved_bits3);
	printAttribute("descriptors_loop_length", (int)descriptors_loop_length);
	printAttribute("reserved_bits4", (int)reserved_bits4);
	printAttribute("transport_stream_loop_length", (int)transport_stream_loop_length);
	printAttribute("crc32", (int)crc32);
	
	printAttribute("descriptor_list_length", (int)descriptor_list_length);
	for(unsigned int i=0; i<descriptor_list_length; i++)
	{
		descriptor_list[i].print();
	}
	
	printAttribute("program_list_length", (int)program_list_length);
	for(unsigned int i=0; i<transport_stream_list_length; i++)
	{
		transport_stream_list[i].print();
	}
}

TsBatPacket::~TsBatPacket()
{
	delete []descriptor_list;
    delete []transport_stream_list;
}

TsNitPacket::TsNitPacket(){}

TsNitPacket::TsNitPacket(unsigned short int offset, unsigned char bytes[188])
{
    /* default */
	descriptor_list_length = 0;
	descriptor_list = new TsDescriptorObject[30];
	
    transport_stream_list_length = 0;
    transport_stream_list = new BatTransportStreamObject[33];
    /* default */

    table_id = bytes[offset++];
    if(table_id != 0x40 && table_id != 0x41)
    {
		//printDebugMessage("table id " + to_str(table_id) + " not handled for pid " + to_str(TS_NIT_PID), WARNING);
		return;
	}
    
    section_syntax_indicator = (bytes[offset] & 0x80) >> 7;
    constant_zero_bit = (bytes[offset] & 0x40) >> 6;
    reserved_bits1 = (bytes[offset] & 0x30) >> 4;

    section_length1 = (bytes[offset++] & 0x0F) << 8;
    section_length2 = bytes[offset++];
    section_length = section_length1 | section_length2;

    network_id1 = bytes[offset++] << 8;
    network_id2 = bytes[offset++];
    network_id = network_id1 | network_id2;

    reserved_bits2 = (bytes[offset] & 0xC0) >> 6;
    version_number = (bytes[offset] & 0x3E) >> 1;
    current_next_indicator = bytes[offset++] & 0x01;
    section_number = bytes[offset++];
    last_section_number = bytes[offset++];
    
    reserved_bits3 = (bytes[offset] & 0xF0) >> 4;
    
    descriptors_loop_length1 = (bytes[offset++] & 0x0F) << 8;
    descriptors_loop_length2 = bytes[offset++];
    descriptors_loop_length = descriptors_loop_length1 | descriptors_loop_length2;
    
    //N LOOP for bouquet descriptors
    unsigned char info_cnt = 0;
    TsDescriptorObject *current_descriptor;
    while(info_cnt < descriptors_loop_length)
    {
		if(offset >= TS_PACKET_SIZE - TS_CRC32_SIZE)
			break;
        current_descriptor = new TsDescriptorObject(offset, bytes);
        descriptor_list[descriptor_list_length++] = *current_descriptor;
        info_cnt += 2 + current_descriptor->descriptor_length;
        offset += 2 + current_descriptor->descriptor_length;
        delete current_descriptor;
    }
    
    reserved_bits4 = (bytes[offset] & 0xF0) >> 4;
    
    transport_stream_loop_length1 = (bytes[offset++] & 0x0F) << 8;
    transport_stream_loop_length2 = bytes[offset++];
    transport_stream_loop_length = transport_stream_loop_length1 | transport_stream_loop_length2;

    //N LOOP for transport streams
    BatTransportStreamObject *current_transport_stream;
    while(offset < section_length + 3)
    {
		if(offset >= TS_PACKET_SIZE - TS_CRC32_SIZE)
			break;
        current_transport_stream = new BatTransportStreamObject(offset, bytes);
        offset += current_transport_stream->getTotalLength();
        transport_stream_list[transport_stream_list_length++] = *current_transport_stream;
        delete current_transport_stream;
    }

    crc321 = bytes[offset++] << 24;
    crc322 = bytes[offset++] << 16;
    crc323 = bytes[offset++] << 8;
    crc324 = bytes[offset++];
    crc32 = crc321 | crc322 | crc323 | crc324;
}

TsNitPacket::TsNitPacket(TsNitPacket &rhs)
{
    table_id = rhs.table_id;
    section_syntax_indicator = rhs.section_syntax_indicator;
    constant_zero_bit = rhs.constant_zero_bit;
    reserved_bits1 = rhs.reserved_bits1;
    section_length = rhs.section_length;
    network_id = rhs.network_id;
    reserved_bits2 = rhs.reserved_bits2;
    version_number = rhs.version_number;
    current_next_indicator = rhs.current_next_indicator;
    section_number = rhs.section_number;
    last_section_number = rhs.last_section_number;
    reserved_bits3 = rhs.reserved_bits3;
    descriptors_loop_length = rhs.descriptors_loop_length;
    reserved_bits4 = rhs.reserved_bits4;
    transport_stream_loop_length = rhs.transport_stream_loop_length;
    crc32 = rhs.crc32;
    
    descriptor_list_length = rhs.descriptor_list_length;
    descriptor_list = new TsDescriptorObject[30];
    for(unsigned int i=0; i<rhs.descriptor_list_length; i++)
    {
		descriptor_list[i] = rhs.descriptor_list[i];
    }
    
    transport_stream_list_length = rhs.transport_stream_list_length;
    transport_stream_list = new BatTransportStreamObject[33];
    for(unsigned int i=0; i<rhs.transport_stream_list_length; i++)
    {
        transport_stream_list[i] = rhs.transport_stream_list[i];
    }
}

TsNitPacket &TsNitPacket::operator=(TsNitPacket &rhs)
{
    table_id = rhs.table_id;
    section_syntax_indicator = rhs.section_syntax_indicator;
    constant_zero_bit = rhs.constant_zero_bit;
    reserved_bits1 = rhs.reserved_bits1;
    section_length = rhs.section_length;
    network_id = rhs.network_id;
    reserved_bits2 = rhs.reserved_bits2;
    version_number = rhs.version_number;
    current_next_indicator = rhs.current_next_indicator;
    section_number = rhs.section_number;
    last_section_number = rhs.last_section_number;
    reserved_bits3 = rhs.reserved_bits3;
    descriptors_loop_length = rhs.descriptors_loop_length;
    reserved_bits4 = rhs.reserved_bits4;
    transport_stream_loop_length = rhs.transport_stream_loop_length;
    crc32 = rhs.crc32;
    
    delete []descriptor_list;
    descriptor_list_length = rhs.descriptor_list_length;
    descriptor_list = new TsDescriptorObject[30];
    for(unsigned int i=0; i<rhs.descriptor_list_length; i++)
    {
		descriptor_list[i] = rhs.descriptor_list[i];
    }
    
    delete []transport_stream_list;
    transport_stream_list_length = rhs.transport_stream_list_length;
    transport_stream_list = new BatTransportStreamObject[33];
    for(unsigned int i=0; i<rhs.transport_stream_list_length; i++)
    {
        transport_stream_list[i] = rhs.transport_stream_list[i];
    }

    return *this;
}

void TsNitPacket::check(){}

void TsNitPacket::print()
{
	if(table_id != 0x40 && table_id != 0x41)
		return;
	printDebugMessage("TS NIT packet attributes:", INFO);
	printAttribute("table_id", (int)table_id);
	printAttribute("section_syntax_indicator", (int)section_syntax_indicator);
	printAttribute("constant_zero_bit", (int)constant_zero_bit);
	printAttribute("reserved_bits1", (int)reserved_bits1);
	printAttribute("section_length", (int)section_length);
	printAttribute("network_id", (int)network_id);
	printAttribute("reserved_bits2", (int)reserved_bits2);
	printAttribute("version_number", (int)version_number);
	printAttribute("current_next_indicator", (int)current_next_indicator);
	printAttribute("section_number", (int)section_number);
	printAttribute("last_section_number", (int)last_section_number);
	printAttribute("reserved_bits3", (int)reserved_bits3);
	printAttribute("descriptors_loop_length", (int)descriptors_loop_length);
	printAttribute("reserved_bits4", (int)reserved_bits4);
	printAttribute("transport_stream_loop_length", (int)transport_stream_loop_length);
	printAttribute("crc32", (int)crc32);
	
	printAttribute("descriptor_list_length", (int)descriptor_list_length);
	for(unsigned int i=0; i<descriptor_list_length; i++)
	{
		descriptor_list[i].print();
	}
	
	printAttribute("transport_stream_list_length", (int)transport_stream_list_length);
	for(unsigned int i=0; i<transport_stream_list_length; i++)
	{
		transport_stream_list[i].print();
	}
}

TsNitPacket::~TsNitPacket()
{
	delete []descriptor_list;
    delete []transport_stream_list;
}


TsSdtPacket::TsSdtPacket(){}

TsSdtPacket::TsSdtPacket(unsigned short int offset, unsigned char bytes[188])
{
	/* default */
	service_list_length = 0;
	service_list = new SdtServiceObject[33];
	/* default */
	
	table_id = bytes[offset++];
	if(table_id != 0x42 && table_id != 0x46)
    {
		return;
	}
	
	section_syntax_indicator = (bytes[offset] & 0x80) >> 7;
	constant_zero_bit = (bytes[offset] & 0x40) >> 6;
	reserved_bits1 = (bytes[offset] & 0x30) >> 4;
	
	section_length1 = (bytes[offset++] & 0x0F) << 8;
	section_length2 = bytes[offset++];
	section_length = section_length1 | section_length2;
	
	transport_stream_id1 = bytes[offset++] << 8;
	transport_stream_id2 = bytes[offset++];
	transport_stream_id = transport_stream_id1 | transport_stream_id2;
	
	reserved_bits2 = (bytes[offset] & 0xC0) >> 6;
	version_number = (bytes[offset] & 0x3E) >> 1;
	current_next_indicator = bytes[offset++] & 0x01;
	section_number = bytes[offset++];
	last_section_number = bytes[offset++];
	
	original_network_id1 = bytes[offset++] << 8;
	original_network_id2 = bytes[offset++];
	original_network_id = original_network_id1 | original_network_id2;

	reserved_byte = bytes[offset++];

	//N LOOP for services
	SdtServiceObject *current_service;
	while(offset < section_length)
	{
		if(offset >= TS_PACKET_SIZE - TS_CRC32_SIZE)
			break;
		current_service = new SdtServiceObject(offset, bytes);
		offset += 5 + current_service->descriptors_loop_length;
		service_list[service_list_length++] = *current_service;
		delete current_service;
	}
	
	crc321 = bytes[offset++] << 24;
	crc322 = bytes[offset++] << 16;
	crc323 = bytes[offset++] << 8;
	crc324 = bytes[offset++];
	crc32 = crc321 | crc322 | crc323 | crc324;
}

TsSdtPacket::TsSdtPacket(TsSdtPacket &rhs)
{
	table_id = rhs.table_id;
	section_syntax_indicator = rhs.section_syntax_indicator;
	constant_zero_bit = rhs.constant_zero_bit;
	reserved_bits1 = rhs.reserved_bits1;
	section_length = rhs.section_length;
	transport_stream_id = rhs.transport_stream_id;
	reserved_bits2 = rhs.reserved_bits2;
	version_number = rhs.version_number;
	current_next_indicator = rhs.current_next_indicator;
	section_number = rhs.section_number;
	last_section_number = rhs.last_section_number;
	original_network_id = rhs.original_network_id;
	reserved_byte = rhs.reserved_byte;
	crc32 = rhs.crc32;
	service_list_length = rhs.service_list_length;
	service_list = new SdtServiceObject[33];
	for(unsigned int i=0; i<rhs.service_list_length; i++)
	{
		service_list[i] = rhs.service_list[i];
	}
}

TsSdtPacket &TsSdtPacket::operator=(TsSdtPacket &rhs)
{
	table_id = rhs.table_id;
	section_syntax_indicator = rhs.section_syntax_indicator;
	constant_zero_bit = rhs.constant_zero_bit;
	reserved_bits1 = rhs.reserved_bits1;
	section_length = rhs.section_length;
	transport_stream_id = rhs.transport_stream_id;
	reserved_bits2 = rhs.reserved_bits2;
	version_number = rhs.version_number;
	current_next_indicator = rhs.current_next_indicator;
	section_number = rhs.section_number;
	last_section_number = rhs.last_section_number;
	original_network_id = rhs.original_network_id;
	reserved_byte = rhs.reserved_byte;
	crc32 = rhs.crc32;
	delete []service_list;
	service_list_length = rhs.service_list_length;
	service_list = new SdtServiceObject[33];
	for(unsigned int i=0; i<rhs.service_list_length; i++)
	{
		service_list[i] = rhs.service_list[i];
	}

	return *this;
}

void TsSdtPacket::check(){}

void TsSdtPacket::print()
{
	if(table_id != 0x42 && table_id != 0x46)
		return;
	printDebugMessage("TS SDT packet attributes:", INFO);
	printAttribute("table_id", (int)table_id);
	printAttribute("section_syntax_indicator", (int)section_syntax_indicator);
	printAttribute("constant_zero_bit", (int)constant_zero_bit);
	printAttribute("reserved_bits1", (int)reserved_bits1);
	printAttribute("section_length", (int)section_length);
	printAttribute("transport_stream_id", (int)transport_stream_id);
	printAttribute("reserved_bits2", (int)reserved_bits2);
	printAttribute("version_number", (int)version_number);
	printAttribute("current_next_indicator", (int)current_next_indicator);
	printAttribute("section_number", (int)section_number);
	printAttribute("last_section_number", (int)last_section_number);
	printAttribute("original_network_id", (int)original_network_id);
	printAttribute("reserved_byte", (int)reserved_byte);
	printAttribute("crc32", (int)crc32);
	printAttribute("service_list_length", service_list_length);
	for(unsigned int i=0; i<service_list_length; i++)
	{
		service_list[i].print();
	}
}

TsSdtPacket::~TsSdtPacket()
{
	delete []service_list;
}

TsPmtPacket::TsPmtPacket(){}

TsPmtPacket::TsPmtPacket(unsigned short int offset, unsigned char bytes[188])
{
	/* default */
	video_stream_list_length = 0;
	audio_stream_list_length = 0;
	private_data_stream_list_length = 0;
	video_stream_list = new PmtElementaryStreamObject[10];
	audio_stream_list = new PmtElementaryStreamObject[10];
	private_data_stream_list = new PmtElementaryStreamObject[10];
	/* default */
	
	table_id = bytes[offset++];
	if(table_id != 0x02)
    {
		//printDebugMessage("table id " + to_str(table_id) + " not handled for pid " + "of PMT", WARNING);
		return;
	}
	section_syntax_indicator = (bytes[offset] & 0x80) >> 7;
	constant_zero_bit = (bytes[offset] & 0x40) >> 6;
	reserved_bits1 = (bytes[offset] & 0x30) >> 4;
	
	section_length1 = (bytes[offset++] & 0x0F) << 8;
	section_length2 = bytes[offset++];
	section_length = section_length1 | section_length2;
	
	program_no1 = bytes[offset++] << 8;
	program_no2 = bytes[offset++];
	program_no = program_no1 | program_no2;
	
	reserved_bits2 = (bytes[offset] & 0xC0) >> 6;
	version_number = (bytes[offset] & 0x3E) >> 1;
	current_next_indicator = bytes[offset++] & 0x01;
	section_number = bytes[offset++];
	last_section_number = bytes[offset++];
	reserved_bits3= (bytes[offset] & 0xE0) >> 5;
	
	pcr_pid1 = (bytes[offset++] & 0x1F) << 8;
	pcr_pid2 = bytes[offset++];
	pcr_pid = pcr_pid1 | pcr_pid2;
	
	reserved_bits4 = (bytes[offset] & 0xF0) >> 4;
	
	prog_info_length1 = (bytes[offset++] & 0x0F) << 8;
	prog_info_length2 = bytes[offset++];
	prog_info_length = prog_info_length1 | prog_info_length2;
	
	//N loop for elementary stream pids
	PmtElementaryStreamObject *current_es;
	while(offset < section_length + 4)
	{
		if(offset >= TS_PACKET_SIZE - TS_CRC32_SIZE)
			break;
		current_es = new PmtElementaryStreamObject(offset, bytes);
		switch(current_es->stream_type)
		{
			case 0x01:
			case 0x02:
			case 0x1B:
				video_stream_list[video_stream_list_length++] = *current_es;
				break;
			case 0x03:
			case 0x04:
			case 0x0F:
			case 0x11:
				audio_stream_list[audio_stream_list_length++] = *current_es;
				break;
			case 0x05:
			case 0x06:
				private_data_stream_list[private_data_stream_list_length++] = *current_es;
				break;
		}
		offset += 5 + current_es->es_info_length;
		delete current_es;
	}
	
	crc321 = bytes[offset++] << 24;
	crc322 = bytes[offset++] << 16;
	crc323 = bytes[offset++] << 8;
	crc324 = bytes[offset++];
	crc32 = crc321 | crc322 | crc323 | crc324;
}

TsPmtPacket::TsPmtPacket(TsPmtPacket &rhs)
{
	table_id = rhs.table_id;
	section_syntax_indicator = rhs.section_syntax_indicator;
	constant_zero_bit = rhs.constant_zero_bit;
	reserved_bits1 = rhs.reserved_bits1;
	section_length = rhs.section_length;
	transport_stream_id = rhs.transport_stream_id;
	reserved_bits2 = rhs.reserved_bits2;
	version_number = rhs.version_number;
	current_next_indicator = rhs.current_next_indicator;
	section_number = rhs.section_number;
	last_section_number = rhs.last_section_number;
	crc32 = rhs.crc32;
	program_no = rhs.program_no;
	reserved_bits3 = rhs.reserved_bits3;
	pcr_pid = rhs.pcr_pid;
	reserved_bits4 = rhs.reserved_bits4;
	prog_info_length = rhs.prog_info_length;
	video_stream_list_length = rhs.video_stream_list_length;
	audio_stream_list_length = rhs.audio_stream_list_length;
	private_data_stream_list_length = rhs.private_data_stream_list_length;
	
	video_stream_list = new PmtElementaryStreamObject[33];
	for(unsigned int i=0; i<rhs.video_stream_list_length; i++)
	{
		video_stream_list[i] = rhs.video_stream_list[i];
	}
	
	audio_stream_list = new PmtElementaryStreamObject[33];
	for(unsigned int i=0; i<rhs.audio_stream_list_length; i++)
	{
		audio_stream_list[i] = rhs.audio_stream_list[i];
	}
	
	private_data_stream_list = new PmtElementaryStreamObject[33];
	for(unsigned int i=0; i<rhs.private_data_stream_list_length; i++)
	{
		private_data_stream_list[i] = rhs.private_data_stream_list[i];
	}
	
}

TsPmtPacket &TsPmtPacket::operator=(TsPmtPacket &rhs)
{
	table_id = rhs.table_id;
	section_syntax_indicator = rhs.section_syntax_indicator;
	constant_zero_bit = rhs.constant_zero_bit;
	reserved_bits1 = rhs.reserved_bits1;
	section_length = rhs.section_length;
	transport_stream_id = rhs.transport_stream_id;
	reserved_bits2 = rhs.reserved_bits2;
	version_number = rhs.version_number;
	current_next_indicator = rhs.current_next_indicator;
	section_number = rhs.section_number;
	last_section_number = rhs.last_section_number;
	crc32 = rhs.crc32;
	program_no = rhs.program_no;
	reserved_bits3 = rhs.reserved_bits3;
	pcr_pid = rhs.pcr_pid;
	reserved_bits4 = rhs.reserved_bits4;
	prog_info_length = rhs.prog_info_length;
	video_stream_list_length = rhs.video_stream_list_length;
	audio_stream_list_length = rhs.audio_stream_list_length;
	private_data_stream_list_length = rhs.private_data_stream_list_length;
	
	delete []video_stream_list;
	video_stream_list = new PmtElementaryStreamObject[33];
	for(unsigned int i=0; i<rhs.video_stream_list_length; i++)
	{
		video_stream_list[i] = rhs.video_stream_list[i];
	}
	
	delete []audio_stream_list;
	audio_stream_list = new PmtElementaryStreamObject[33];
	for(unsigned int i=0; i<rhs.audio_stream_list_length; i++)
	{
		audio_stream_list[i] = rhs.audio_stream_list[i];
	}
	
	delete []private_data_stream_list;
	private_data_stream_list = new PmtElementaryStreamObject[33];
	for(unsigned int i=0; i<rhs.private_data_stream_list_length; i++)
	{
		private_data_stream_list[i] = rhs.private_data_stream_list[i];
	}
	
	return *this;
}

void TsPmtPacket::check(){}

void TsPmtPacket::print()
{
	if(table_id != 0x02)
		return;
	printDebugMessage("TS PMT packet attributes:", INFO);
	printAttribute("table_id", (int)table_id);
	printAttribute("section_syntax_indicator", (int)section_syntax_indicator);
	printAttribute("constant_zero_bit", (int)constant_zero_bit);
	printAttribute("reserved_bits1", (int)reserved_bits1);
	printAttribute("section_length", (int)section_length);
	printAttribute("reserved_bits2", (int)reserved_bits2);
	printAttribute("version_number", (int)version_number);
	printAttribute("current_next_indicator", (int)current_next_indicator);
	printAttribute("section_number", (int)section_number);
	printAttribute("last_section_number", (int)last_section_number);
	printAttribute("reserved_bits3", (int)reserved_bits3);
	printAttribute("pcr_pid", (int)pcr_pid);
	printAttribute("reserved_bits4", (int)reserved_bits4);
	printAttribute("prog_info_length", (int)prog_info_length);
	printAttribute("crc32", (int)crc32);
	for(unsigned short int i=0; i<video_stream_list_length; i++)
	{
		video_stream_list[i].print();
	}
	for(unsigned short int i=0; i<audio_stream_list_length; i++)
	{
		audio_stream_list[i].print();
	}
	for(unsigned short int i=0; i<private_data_stream_list_length; i++)
	{
		private_data_stream_list[i].print();
	}
}

TsPmtPacket::~TsPmtPacket()
{
	delete []video_stream_list;
	delete []audio_stream_list;
	delete []private_data_stream_list;
}

TsPesPacket::TsPesPacket()
{
	header = NULL;
}

TsPesPacket::TsPesPacket(unsigned short int offset, unsigned char bytes[188])
{
	header = new TsPesHeader(offset, bytes);
	if(header->hasError)
	{
		hasError = true;
	}
	else if(header->isPesStarted)
	{
		offset += PES_HEADER_SIZE;
		pes_payload_start_offset = offset;
	}
	else
	{
		pes_payload_start_offset = offset;
	}
	/*int start_offset = pes_payload_start_offset + PES_HEADER_FLAG_SIZE + header->PES_header_data_len;
	int len = TS_PACKET_SIZE - start_offset;
	writeRawData("2004.es", bytes + start_offset, len);
	cout << "number of bytes written: " << len << endl;*/
}

TsPesPacket::TsPesPacket(TsPesPacket &rhs)
{
	hasError = rhs.hasError;
	pes_payload_start_offset = rhs.pes_payload_start_offset;
	*header = *(rhs.header);
}

TsPesPacket &TsPesPacket::operator=(TsPesPacket &rhs)
{
	hasError = rhs.hasError;
	pes_payload_start_offset = rhs.pes_payload_start_offset;
	delete header;
	*header = *(rhs.header);

	return *this;
}

void TsPesPacket::check(){}

void TsPesPacket::print()
{
	if(hasError)
		return;
	printDebugMessage("TS PES packet attributes:", INFO);
	printAttribute("pes_payload_start_offset", (int)pes_payload_start_offset);
	header->print();
}

TsPesPacket::~TsPesPacket()
{
	delete header;
}
