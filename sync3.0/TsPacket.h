#ifndef _TS_PACKET_H
#define _TS_PACKET_H 

#include "TsPacket.def"
#include "globals.h"

typedef enum adaptation_field_control_type
{
	reserved_adaptation_field_control,
	no_adaptation_field_payload_only,
	no_payload_adaptation_field_only,
	adaptation_field_and_payload
} adaptation_field_control_type_t;

const std::string adaptation_field_control_type_names[4] = {
	"reserved_adaptation_field_control",
	"no_adaptation_field_payload_only",
	"no_payload_adaptation_field_only",
	"adaptation_field_and_payload"
};

typedef enum pts_dts_flag_status
{
	no_pts_dts,
	forbidden,
	pts_only,
	pts_and_dts
} pts_dts_flag_status_t;

const std::string pts_dts_flag_status_names[4] = {
	"no_pts_dts",
	"forbidden",
	"pts_only",
	"pts_and_dts"
};

typedef enum ts_packet_type
{
	TS_PAT_PACKET,
	TS_CAT_PACKET,
	TS_TSDT_PACKET,
	TS_NIT_PACKET,
	TS_SDT_PACKET,
	TS_EIT_PACKET,
	TS_BAT_PACKET,
	TS_PMT_PACKET,
	TS_PES_VIDEO_PACKET,
	TS_PES_AUDIO_PACKET,
	TS_PES_PRIVATE_DATA_PACKET,
	TS_PCR_PACKET,
	TS_NULL_PACKET,
	TS_UNKNOWN_PACKET
} ts_packet_type_t;

const std::string ts_packet_type_names[14] = {
	"TS_PAT_PACKET",
	"TS_CAT_PACKET",
	"TS_TSDT_PACKET",
	"TS_NIT_PACKET",
	"TS_SDT_PACKET",
	"TS_EIT_PACKET",
	"TS_BAT_PACKET",
	"TS_PMT_PACKET",
	"TS_PES_VIDEO_PACKET",
	"TS_PES_AUDIO_PACKET",
	"TS_PES_PRIVATE_DATA_PACKET",
	"TS_PCR_PACKET",
	"TS_NULL_PACKET",
	"TS_UNKNOWN_PACKET"
};

typedef enum ts_packet_status
{
	NO_ERROR,
	TRANSPORT_ERROR,
	ADAPTATION_FIELD_ERROR,
	NULL_PACKET_STATUS,
	UNKNOWN_PACKET_STATUS
} ts_packet_status_t;

const std::string ts_packet_status_names[5] = {
	"NO_ERROR",
	"TRANSPORT_ERROR",
	"ADAPTATION_FIELD_ERROR",
	"NULL_PACKET_STATUS",
	"UNKNOWN_PACKET_STATUS"
};

typedef enum es_source_types
{
	ES_SOURCE_DVB,
	ES_SOURCE_P2P
} es_source_types_t;

const std::string es_source_type_names[2] = {
	"ES_SOURCE_DVB",
	"ES_SOURCE_P2P"
};

typedef enum es_types
{
	ES_ALL,
	ES_VIDEO,
	ES_AUDIO,
	ES_PRIVATE_DATA
} es_types_t;

const std::string es_type_names[4] = {
	"ES_ALL",
	"ES_VIDEO",
	"ES_AUDIO",
	"ES_PRIVATE_DATA"
};

typedef enum es_subtypes
{
	ES_BASE,
	ES_ENHANCEMENT
} es_subtypes_t;

const std::string es_subtype_names[2] = {
	"ES_BASE",
	"ES_ENHANCEMENT"
};

class TsDescriptorObject
{
	private:
	
		
	
	public:
	
		unsigned char descriptor_tag;
		unsigned char descriptor_length;
		char *descriptor;
	
		TsDescriptorObject();
		TsDescriptorObject(unsigned short int offset, unsigned char bytes[188]);
		TsDescriptorObject(TsDescriptorObject &rhs);
		TsDescriptorObject &operator=(TsDescriptorObject &rhs);
		void print();
		~TsDescriptorObject();
};

class TsUrlExtension
{
	private:
	public:
		unsigned char extension_id;
		unsigned char reserved_byte;
		unsigned char URL_extension_length;
		unsigned char *URL_extension;

		TsUrlExtension();
		TsUrlExtension(unsigned short int offset, unsigned char bytes[188]);
		TsUrlExtension(TsUrlExtension &rhs);
		TsUrlExtension &operator=(TsUrlExtension &rhs);
		void print();
		~TsUrlExtension();
};

class PatProgramObject
{
	private:
	
		unsigned short int program_no1;
		unsigned short int program_no2;
		
		unsigned short int pmt_pid1;
		unsigned short int pmt_pid2;
	
	public:
	
		unsigned short int program_no;
		unsigned char empty_bits;
		unsigned short int pmt_pid;
		
		PatProgramObject();
		PatProgramObject(unsigned short int offset, unsigned char bytes[188]);
		void print();
		~PatProgramObject();
};

class EitEventObject
{
    private:

        unsigned short int event_id1;
        unsigned short int event_id2;

        unsigned long long int start_time1;
        unsigned long long int start_time2;
        unsigned long long int start_time3;
        unsigned long long int start_time4;
        unsigned long long int start_time5;

        unsigned int duration1;
        unsigned int duration2;
        unsigned int duration3;

        unsigned short int descriptors_loop_length1;
        unsigned short int descriptors_loop_length2;

    public:

        unsigned short int event_id;
        long long int start_time;
        unsigned int duration;
        unsigned char running_status;
        bool free_CA_mode;
        unsigned short int descriptors_loop_length;
        TsDescriptorObject *descriptor_list;
        unsigned int descriptor_list_length;

        EitEventObject();
        EitEventObject(unsigned short int offset, unsigned char bytes[188]);
        EitEventObject(EitEventObject &rhs);
        EitEventObject &operator=(EitEventObject &rhs);
        unsigned short int getTotalLength();
        void print();
        ~EitEventObject();
};

class BatTransportStreamObject
{
    private:

        unsigned short int transport_stream_id1;
        unsigned short int transport_stream_id2;

        unsigned short int original_network_id1;
        unsigned short int original_network_id2;
        
        unsigned short int descriptors_loop_length1;
        unsigned short int descriptors_loop_length2;

    public:

        unsigned short int transport_stream_id;
        long long int original_network_id;
        unsigned int reserved_for_future_purpose_bits;
        unsigned short int descriptors_loop_length;
        TsDescriptorObject *descriptor_list;
        unsigned int descriptor_list_length;

        BatTransportStreamObject();
        BatTransportStreamObject(unsigned short int offset, unsigned char bytes[188]);
        BatTransportStreamObject(BatTransportStreamObject &rhs);
        BatTransportStreamObject &operator=(BatTransportStreamObject &rhs);
        unsigned short int getTotalLength();
        void print();
        ~BatTransportStreamObject();
};

class TsAdditionalContentUrlDescriptorObject
{
	private:
	
		unsigned short int additional_content_id1;
		unsigned short int additional_content_id2;
	
	public:

		unsigned char descriptor_tag;
		unsigned char descriptor_length;
		unsigned char descriptor_tag_extension;
		unsigned char reserved_byte;
		unsigned short int additional_content_id;
		unsigned char URL_base_length;
		unsigned char *URL_base;
		unsigned char URL_extension_count;
		TsUrlExtension *URL_extensions;

		TsAdditionalContentUrlDescriptorObject();
		TsAdditionalContentUrlDescriptorObject(unsigned short int offset, unsigned char bytes[188]);
		TsAdditionalContentUrlDescriptorObject(TsAdditionalContentUrlDescriptorObject &rhs);
		TsAdditionalContentUrlDescriptorObject &operator=(TsAdditionalContentUrlDescriptorObject &rhs);
		std::string getUrl();
		void print();
		~TsAdditionalContentUrlDescriptorObject();
};

class SdtServiceObject
{
	private:
		unsigned short int service_id1;
		unsigned short int service_id2;

		unsigned short int descriptors_loop_length1;
		unsigned short int descriptors_loop_length2;
	public:
		unsigned short int service_id;
		unsigned char reserved_bits1;
		bool EIT_schedule_flag;
		bool EIT_present_following_flag;
		unsigned char running_status;
		bool free_CA_mode;
		unsigned short int descriptors_loop_length;
		TsDescriptorObject *descriptor_list;
		unsigned int descriptor_list_length;
		TsAdditionalContentUrlDescriptorObject *additional_content_url_descriptor;

		SdtServiceObject();
		SdtServiceObject(unsigned short int offset, unsigned char bytes[188]);
		SdtServiceObject(SdtServiceObject &rhs);
		SdtServiceObject &operator=(SdtServiceObject &rhs);
		void print();
		~SdtServiceObject();
};

class PmtElementaryStreamObject
{
	private:
	
		unsigned short int elementary_pid1;
		unsigned short int elementary_pid2;
		
		unsigned short int es_info_length1;
		unsigned short int es_info_length2;
	
	public:
	
		unsigned char stream_type;
		unsigned char reserved_bits1;
		unsigned short int elementary_pid;
		unsigned char reserved_bits2;
		unsigned short int es_info_length;
		TsDescriptorObject *descriptor_list;
		unsigned int descriptor_list_length;
		
		PmtElementaryStreamObject();
		PmtElementaryStreamObject(unsigned short int offset, unsigned char bytes[188]);
		PmtElementaryStreamObject(PmtElementaryStreamObject &rhs);
		PmtElementaryStreamObject &operator=(PmtElementaryStreamObject &rhs);
		void print();
		~PmtElementaryStreamObject();
};

class TsAdaptationFieldExtension
{
	private:
	
		unsigned short int ltw_offset1;
		unsigned short int ltw_offset2;
		
		unsigned int piecewise_rate1;
		unsigned int piecewise_rate2;
		unsigned int piecewise_rate3;
		
		unsigned int data0;
		unsigned int data1;
		unsigned int data2;
		unsigned int data3;
		unsigned int data4;
		
		unsigned int DTS_next_AU_1;
		unsigned int DTS_next_AU_2;
		unsigned int DTS_next_AU_3;
		
		bool DTS_next_AU_marker1;
		bool DTS_next_AU_marker2;
		bool DTS_next_AU_marker3;
		
	public:
	
		unsigned char adaptation_field_extension_len;
		bool ltw_flag;
		bool piecewise_rate_flag;
		bool seamless_splice_flag;
		unsigned char extension_reserved;
		
		bool ltw_valid_flag;
		unsigned short int ltw_offset;
		
		unsigned char piecewise_rate_reserved;
		unsigned int piecewise_rate;
		
		int splice_type;
		unsigned int DTS_next_AU;
		
		TsAdaptationFieldExtension();
		TsAdaptationFieldExtension(unsigned short int offset, unsigned char bytes[188]);
		void check();
		void print();
		~TsAdaptationFieldExtension();
};

class TsHeader
{
	private:
	
		unsigned short int pid1;
		unsigned short int pid2;
	
	public:
	
		unsigned char sync_byte;
		bool transport_error_indicator;
		bool payload_unit_start_indicator;
		bool transport_priority;
		unsigned short int pid;
		unsigned char transport_scrambling_control;
		adaptation_field_control_type_t adaptation_field_control;
		unsigned char continuity_counter;
		
		TsHeader();
		TsHeader(unsigned short int offset, unsigned char bytes[188]);
		void check();
		void print();
		~TsHeader();
};

class TsPesHeader
{
	private:
	
		bool payload_started;
	
		unsigned int packet_start_code_prefix1;
		unsigned int packet_start_code_prefix2;
		unsigned int packet_start_code_prefix3;
		
		unsigned short int PES_packet_length1;
		unsigned short int PES_packet_length2;
		
		unsigned char pts_data1;
		unsigned char pts_data2;
		unsigned char pts_data3;
		unsigned char pts_data4;
		unsigned char pts_data5;
		
		unsigned int PTS1;
		unsigned int PTS2;
		unsigned int PTS3;
		
		unsigned char dts_data1;
		unsigned char dts_data2;
		unsigned char dts_data3;
		unsigned char dts_data4;
		unsigned char dts_data5;
		
		unsigned int DTS1;
		unsigned int DTS2;
		unsigned int DTS3;
	
	public:
	
		bool isPesStarted;
		bool hasError;
		
		unsigned int packet_start_code_prefix;
		unsigned char stream_id;
		unsigned short int PES_packet_length;
		
		unsigned char reserved_bits;
		unsigned char PES_scrambling_control;
		bool PES_priority;
		bool data_alignment_indicator;
		bool copyright;
		bool original_or_copy;
		unsigned char pts_dts_flag;
		bool escr_flag;
		bool es_rate_flag;
		bool DSM_trick_mode_flag;
		bool additional_copy_info_flag;
		bool previous_PES_crc_flag;
		bool PES_extension_flag;
		unsigned char PES_header_data_len;
		
		unsigned int PTS;
		unsigned int DTS;
		
		bool PTS_marker_bit1;
		bool PTS_marker_bit2;
		bool PTS_marker_bit3;

		bool DTS_marker_bit1;
		bool DTS_marker_bit2;
		bool DTS_marker_bit3;
		
		TsPesHeader();
		TsPesHeader(unsigned short int offset, unsigned char bytes[188], bool _payload_started);
		void check();
		bool hasPTS();
		void print();
		~TsPesHeader();
};

class TsAdaptationField
{
	private:
	
		unsigned long long PCR_base1;
		unsigned long long PCR_base2;
		unsigned long long PCR_base3;
		unsigned long long PCR_base4;
		unsigned long long PCR_base5;
		
		unsigned short int PCR_extension1;
		unsigned short int PCR_extension2;
		
		unsigned long long OPCR_base1;
		unsigned long long OPCR_base2;
		unsigned long long OPCR_base3;
		unsigned long long OPCR_base4;
		unsigned long long OPCR_base5;
		
		unsigned short int OPCR_extension1;
		unsigned short int OPCR_extension2;
	
	public:
	
		bool discontinuity_indicator;
		bool random_access_indicator;
		bool elementary_stream_priority_indicator;
		bool PCR_flag;
		bool OPCR_flag;
		bool splicing_point_flag;
		bool transport_private_data_flag;
		bool adaptation_field_extension_flag;
		
		unsigned long long PCR_base;
		unsigned char PCR_reserved;
		unsigned short int PCR_extension;
		
		unsigned long long OPCR_base;
		unsigned char OPCR_reserved;
		unsigned short int OPCR_extension;
		
		unsigned char splice_countdown;
		
		unsigned char transport_private_data_length;
		unsigned char *private_data_bytes;
		
		TsAdaptationFieldExtension *extension;
		
		TsAdaptationField();
		TsAdaptationField(TsAdaptationField &rhs);
		TsAdaptationField &operator=(TsAdaptationField &rhs);
		TsAdaptationField(unsigned short int offset, unsigned char bytes[188]);
		void check();
		void print();
		~TsAdaptationField();
};

class TsPayloadBase
{
	public:
	
		/* SI tables common members */
		unsigned char table_id;
		bool section_syntax_indicator;
		bool constant_zero_bit;
		unsigned char reserved_bits1;
		unsigned short int section_length;
		unsigned short int transport_stream_id;
		unsigned char reserved_bits2;
		unsigned char version_number;
		bool current_next_indicator;
		unsigned char section_number;
		unsigned char last_section_number;
		unsigned int crc32;
		unsigned char reserved_bits3;
		unsigned char reserved_bits4;
		
		/* TsPatPacket specific members */
		PatProgramObject *program_list;
		unsigned int program_list_length;
		unsigned short int nit_pid;
		
		/* TsPmtPacket specific members */
		unsigned short int program_no;
		unsigned short int pcr_pid;
		unsigned short int prog_info_length;
		PmtElementaryStreamObject *video_stream_list;
		unsigned int video_stream_list_length;
		PmtElementaryStreamObject *audio_stream_list;
		unsigned int audio_stream_list_length;
		PmtElementaryStreamObject *private_data_stream_list;
		unsigned int private_data_stream_list_length;

		/* TsSdtPacket specific members */
		SdtServiceObject *service_list;
		unsigned int service_list_length;
		unsigned short int original_network_id;
		unsigned char reserved_byte;
		
		/* TsEitPacket specific members */
        unsigned char segment_last_section_number;
        unsigned char last_table_id;
        unsigned short int service_id;
        EitEventObject *event_list;
        unsigned int event_list_length;
        
        /* TsBatPacket&TsNitPacket specific members */
        unsigned short int descriptors_loop_length;
        TsDescriptorObject *descriptor_list;
        unsigned int descriptor_list_length;
        unsigned short int transport_stream_loop_length;
        BatTransportStreamObject *transport_stream_list;
        unsigned int transport_stream_list_length;
        
        /* TsBatPacket specific members */
        unsigned short int bouquet_id;
        
        /* TsNitPacket specific members */
        unsigned short int network_id;
		
		/* TsPesPacket specific members */
		TsPesHeader *header;
		unsigned int pes_payload_start_offset;
		bool payload_started;
		
		/* common members */
		bool hasError;
		
		TsPayloadBase();
		TsPayloadBase(unsigned short int offset, unsigned char bytes[188]);
		virtual void check() = 0;
		std::string getUrl();
		bool isStartOfFrame();
		bool isNewFrame(unsigned int PTS);
		virtual void print() = 0;
		virtual ~TsPayloadBase();
};

class TsPatPacket : public TsPayloadBase
{
	private:
	
		unsigned short int section_length1;
		unsigned short int section_length2;
		
		unsigned short int transport_stream_id1;
		unsigned short int transport_stream_id2;
		
		unsigned int crc321;
		unsigned int crc322;
		unsigned int crc323;
		unsigned int crc324;
	
	public:
	
		TsPatPacket();
		TsPatPacket(unsigned short int offset, unsigned char bytes[188]);
		TsPatPacket(TsPatPacket &rhs);
		TsPatPacket &operator=(TsPatPacket &rhs);
		void check();
		void print();
		~TsPatPacket();
};

class TsSdtPacket : public TsPayloadBase
{
	private:
	
		unsigned short int section_length1;
		unsigned short int section_length2;
		
		unsigned short int transport_stream_id1;
		unsigned short int transport_stream_id2;

		unsigned short int original_network_id1;
		unsigned short int original_network_id2;
		
		unsigned int crc321;
		unsigned int crc322;
		unsigned int crc323;
		unsigned int crc324;
	
	public:
	
		TsSdtPacket();
		TsSdtPacket(unsigned short int offset, unsigned char bytes[188]);
		TsSdtPacket(TsSdtPacket &rhs);
		TsSdtPacket &operator=(TsSdtPacket &rhs);
		void check();
		void print();
		~TsSdtPacket();
};

class TsEitPacket : public TsPayloadBase
{
    private:

        unsigned short int section_length1;
        unsigned short int section_length2;

        unsigned short int transport_stream_id1;
        unsigned short int transport_stream_id2;

        unsigned short int service_id1;
        unsigned short int service_id2;

        unsigned short int original_network_id1;
        unsigned short int original_network_id2;

        unsigned int crc321;
        unsigned int crc322;
        unsigned int crc323;
        unsigned int crc324;


    public:

        TsEitPacket();
        TsEitPacket(unsigned short int offset, unsigned char bytes[188]);
        TsEitPacket(TsEitPacket &rhs);
        TsEitPacket &operator=(TsEitPacket &rhs);
        void check();
        void print();

        ~TsEitPacket();
};

class TsBatPacket : public TsPayloadBase
{
    private:

        unsigned short int section_length1;
        unsigned short int section_length2;

        unsigned short int bouquet_id1;
        unsigned short int bouquet_id2;

        unsigned short int service_id1;
        unsigned short int service_id2;

        unsigned short int descriptors_loop_length1;
        unsigned short int descriptors_loop_length2;
        
        unsigned short int transport_stream_loop_length1;
        unsigned short int transport_stream_loop_length2;

        unsigned int crc321;
        unsigned int crc322;
        unsigned int crc323;
        unsigned int crc324;


    public:

        TsBatPacket();
        TsBatPacket(unsigned short int offset, unsigned char bytes[188]);
        TsBatPacket(TsBatPacket &rhs);
        TsBatPacket &operator=(TsBatPacket &rhs);
        void check();
        void print();
        ~TsBatPacket();
};

class TsNitPacket : public TsPayloadBase
{
    private:

        unsigned short int section_length1;
        unsigned short int section_length2;

        unsigned short int network_id1;
        unsigned short int network_id2;

        unsigned short int service_id1;
        unsigned short int service_id2;

        unsigned short int descriptors_loop_length1;
        unsigned short int descriptors_loop_length2;
        
        unsigned short int transport_stream_loop_length1;
        unsigned short int transport_stream_loop_length2;

        unsigned int crc321;
        unsigned int crc322;
        unsigned int crc323;
        unsigned int crc324;


    public:

        TsNitPacket();
        TsNitPacket(unsigned short int offset, unsigned char bytes[188]);
        TsNitPacket(TsNitPacket &rhs);
        TsNitPacket &operator=(TsNitPacket &rhs);
        void check();
        void print();
        ~TsNitPacket();
};

class TsPmtPacket : public TsPayloadBase
{
	private:
	
		unsigned short int section_length1;
		unsigned short int section_length2;
		
		unsigned short int program_no1;
		unsigned short int program_no2;
		
		unsigned short int pcr_pid1;
		unsigned short int pcr_pid2;
		
		unsigned short int prog_info_length1;
		unsigned short int prog_info_length2;
		
		unsigned int crc321;
		unsigned int crc322;
		unsigned int crc323;
		unsigned int crc324;
		
	public:
	
		TsPmtPacket();
		TsPmtPacket(unsigned short int offset, unsigned char bytes[188]);
		TsPmtPacket(TsPmtPacket &rhs);
		TsPmtPacket &operator=(TsPmtPacket &rhs);
		void check();
		void print();
		~TsPmtPacket();
};

class TsPesPacket : public TsPayloadBase
{
	private:
	
	public:
	
		TsPesPacket();
		TsPesPacket(unsigned short int offset, unsigned char bytes[188], bool _payload_started);
		TsPesPacket(TsPesPacket &rhs);
		TsPesPacket &operator=(TsPesPacket &rhs);
		void check();
		void print();
		~TsPesPacket();
};

class TsPacket
{
	private:
	
		unsigned short int offset;
		ts_packet_status_t getPacketStatus();
		
	public:

		unsigned char *bytes;
		TsHeader *header;
		unsigned char adaptation_field_length;
		TsAdaptationField *adaptation_field;
		ts_packet_type packet_type;
		ts_packet_status packet_status;
		
		TsPayloadBase *packet_payload;
		
		TsPacket();
		TsPacket(unsigned char *_bytes);
		TsPacket(TsPacket &rhs);
		TsPacket &operator=(TsPacket &rhs);
		bool operator==(const TsPacket &rhs);
		bool operator!=(const TsPacket &rhs);
		void setPacketType();
		void setPacketType(ts_packet_type_t type);
		bool isParseable();
		void parse();
		void check();
		bool hasPCR();
		unsigned long long getPCRbase();
		unsigned short int getPCRextension();
		void print();
		void printBytes();
		~TsPacket();
};

#endif
