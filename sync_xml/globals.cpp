#include <iostream>
#include "globals.h"

using namespace std;

int server_port_no;
bool do_not_sync;
bool do_not_consume;
bool include_ui;
bool print_fill_ratio;

int prebuffering_as_seconds;
int buffer_size_as_seconds;
int buffer_lower_threshold_percentage;
int buffer_upper_threshold_percentage;
bool write_output_to_file_flag;
bool write_input_to_file_flag;

int video_pid_base;
bool video_pid_iterative;
std::string video_decoder_ip_base;
std::string video_secondary_decoder_ip_base;
int video_decoder_port_base;
int video_secondary_decoder_port_base;
std::string video_decoder_protocol;
int number_of_primary_streams;

std::string video_renderer_ip;
int video_renderer_port;
std::string video_renderer_protocol;

int audio_pid_base;
bool audio_pid_iterative;
std::string audio_decoder_ip_base;
int audio_decoder_port_base;
std::string audio_decoder_protocol;
std::string audio_renderer_ip;
int audio_renderer_port;
std::string audio_renderer_protocol;
std::string audio_secondary_decoder_ip_base;
int audio_secondary_decoder_port_base;
std::string audio_secondary_decoder_protocol;
std::string audio_secondary_renderer_ip;
int audio_secondary_renderer_port;
std::string audio_secondary_renderer_protocol;

int private_data_pid_base;
bool private_data_pid_iterative;
std::string private_data_decoder_ip_base;
int private_data_decoder_port_base;
std::string private_data_decoder_protocol;

std::string private_data_renderer_ip;
int private_data_renderer_port;
std::string private_data_renderer_protocol;

std::string vlc_player_ip;
bool add_enhancement_flag;
bool send_pcr_flag;

bool output_for_vlc_flag;
bool no_dvb_flag;
int number_of_dvb_receivers;
int number_of_p2p_video_base_receivers;
int number_of_p2p_video_enhancement_receivers;
int number_of_p2p_audio_receivers;
int number_of_p2p_private_data_receivers;

std::string overlay_client_ip;
int overlay_client_port;
std::string overlay_client_protocol;
bool collaborative_sync_flag;

bool print_measurement_values_flag;
int print_measurement_values_period_as_seconds;

bool file_read_mode_flag;
int number_of_file_receivers;
std::string stream_file_name;

bool additional_content_url_flag;

bool no_video_flag;
bool no_audio_flag;
bool no_private_data_flag;

int pts_factor;
int pts_divisor;
int pts_adder;
int pts_extractor;
int sync_timeout_as_seconds;
int constant_delay;

bool ts_parser_mode;
int selected_program;
bool print_as_hex;
bool print_pat_details;
bool print_cat_details;
bool print_tsdt_details;
bool print_nit_details;
bool print_sdt_details;
bool print_eit_details;
bool print_bat_details;
bool print_pmt_details;
bool print_pes_video_details;
bool print_pes_audio_details;
bool print_pes_private_data_details;
bool print_pcr_details;
bool print_null_details;
bool print_unknown_details;

void configureAll()
{
	ConfigFileParser *conf_file = new ConfigFileParser("sync.conf", ':');
	
	server_port_no = conf_file->getIntValueByName("server_port_no");
	do_not_sync = conf_file->getBoolValueByName("do_not_sync");
	do_not_consume = conf_file->getBoolValueByName("do_not_consume");
	include_ui = conf_file->getBoolValueByName("include_ui");
	print_fill_ratio = conf_file->getBoolValueByName("print_fill_ratio");
	
	prebuffering_as_seconds = conf_file->getIntValueByName("prebuffering_as_seconds");
	buffer_size_as_seconds = conf_file->getIntValueByName("buffer_size_as_seconds");
	buffer_lower_threshold_percentage = conf_file->getIntValueByName("buffer_lower_threshold_percentage");
	buffer_upper_threshold_percentage = conf_file->getIntValueByName("buffer_upper_threshold_percentage");
	write_output_to_file_flag = conf_file->getBoolValueByName("write_output_to_file_flag");
	write_input_to_file_flag = conf_file->getBoolValueByName("write_input_to_file_flag");
	
	video_pid_base = conf_file->getIntValueByName("video_pid_base");
	video_pid_iterative = conf_file->getBoolValueByName("video_pid_iterative");
	video_decoder_ip_base = conf_file->getStrValueByName("video_decoder_ip_base");
	video_decoder_port_base = conf_file->getIntValueByName("video_decoder_port_base");
	video_secondary_decoder_ip_base = conf_file->getStrValueByName("video_secondary_decoder_ip_base");
	video_secondary_decoder_port_base = conf_file->getIntValueByName("video_secondary_decoder_port_base");
	number_of_primary_streams = conf_file->getIntValueByName("number_of_primary_streams");

	video_decoder_protocol = conf_file->getStrValueByName("video_decoder_protocol");
	video_renderer_ip = conf_file->getStrValueByName("video_renderer_ip");
	video_renderer_port = conf_file->getIntValueByName("video_renderer_port");
	video_renderer_protocol = conf_file->getStrValueByName("video_renderer_protocol");

	audio_pid_base = conf_file->getIntValueByName("audio_pid_base");
	audio_pid_iterative = conf_file->getBoolValueByName("audio_pid_iterative");
	audio_decoder_ip_base = conf_file->getStrValueByName("audio_decoder_ip_base");
	audio_decoder_port_base = conf_file->getIntValueByName("audio_decoder_port_base");
	audio_decoder_protocol = conf_file->getStrValueByName("audio_decoder_protocol");
	audio_renderer_ip = conf_file->getStrValueByName("audio_renderer_ip");
	audio_renderer_port = conf_file->getIntValueByName("audio_renderer_port");
	audio_renderer_protocol = conf_file->getStrValueByName("audio_renderer_protocol");

	audio_secondary_decoder_ip_base = conf_file->getStrValueByName("audio_secondary_decoder_ip_base");
	audio_secondary_decoder_port_base = conf_file->getIntValueByName("audio_secondary_decoder_port_base");
	audio_secondary_decoder_protocol = conf_file->getStrValueByName("audio_secondary_decoder_protocol");
	audio_secondary_renderer_ip = conf_file->getStrValueByName("audio_secondary_renderer_ip");
	audio_secondary_renderer_port = conf_file->getIntValueByName("audio_secondary_renderer_port");
	audio_secondary_renderer_protocol = conf_file->getStrValueByName("audio_secondary_renderer_protocol");

	private_data_pid_base = conf_file->getIntValueByName("private_data_pid_base");
	private_data_pid_iterative = conf_file->getBoolValueByName("private_data_pid_iterative");
	private_data_decoder_ip_base = conf_file->getStrValueByName("private_data_decoder_ip_base");
	private_data_decoder_port_base = conf_file->getIntValueByName("private_data_decoder_port_base");
	private_data_decoder_protocol = conf_file->getStrValueByName("private_data_decoder_protocol");
	
	private_data_renderer_ip = conf_file->getStrValueByName("private_data_renderer_ip");
	private_data_renderer_port = conf_file->getIntValueByName("private_data_renderer_port");
	private_data_renderer_protocol = conf_file->getStrValueByName("private_data_renderer_protocol");

	vlc_player_ip = conf_file->getStrValueByName("vlc_player_ip");
	add_enhancement_flag = conf_file->getBoolValueByName("add_enhancement_flag");
	send_pcr_flag = conf_file->getBoolValueByName("send_pcr_flag");

	output_for_vlc_flag = conf_file->getBoolValueByName("output_for_vlc_flag");
	no_dvb_flag = conf_file->getBoolValueByName("no_dvb_flag");
	number_of_dvb_receivers = (int)(!no_dvb_flag);
	number_of_p2p_video_base_receivers = conf_file->getIntValueByName("number_of_p2p_video_base_receivers");
	number_of_p2p_video_enhancement_receivers = add_enhancement_flag ? number_of_p2p_video_base_receivers : 0;
	number_of_p2p_audio_receivers = conf_file->getIntValueByName("number_of_p2p_audio_receivers");
	number_of_p2p_private_data_receivers = conf_file->getIntValueByName("number_of_p2p_private_data_receivers");

	overlay_client_ip = conf_file->getStrValueByName("overlay_client_ip");
	overlay_client_port = conf_file->getIntValueByName("overlay_client_port");
	overlay_client_protocol = conf_file->getStrValueByName("overlay_client_protocol");
	collaborative_sync_flag = conf_file->getBoolValueByName("collaborative_sync_flag");

	print_measurement_values_flag = conf_file->getBoolValueByName("print_measurement_values_flag");
	print_measurement_values_period_as_seconds = conf_file->getIntValueByName("print_measurement_values_period_as_seconds");

	file_read_mode_flag = conf_file->getBoolValueByName("file_read_mode_flag");
	number_of_file_receivers = (int)(file_read_mode_flag);
	stream_file_name = conf_file->getStrValueByName("stream_file_name");
	
	additional_content_url_flag = conf_file->getBoolValueByName("additional_content_url_flag");
	no_video_flag = conf_file->getBoolValueByName("no_video_flag");
	no_audio_flag = conf_file->getBoolValueByName("no_audio_flag");
	no_private_data_flag = conf_file->getBoolValueByName("no_private_data_flag");
	
	pts_factor = conf_file->getIntValueByName("pts_factor");
	pts_divisor = conf_file->getIntValueByName("pts_divisor");
	pts_adder = conf_file->getIntValueByName("pts_adder");
	pts_extractor = conf_file->getIntValueByName("pts_extractor");
	sync_timeout_as_seconds = conf_file->getIntValueByName("sync_timeout_as_seconds");
	constant_delay = conf_file->getIntValueByName("constant_delay");
	
	ts_parser_mode = conf_file->getBoolValueByName("ts_parser_mode");
	selected_program = conf_file->getIntValueByName("selected_program");
	print_as_hex = conf_file->getBoolValueByName("print_as_hex");
	print_pat_details = conf_file->getBoolValueByName("print_pat_details");
	print_cat_details = conf_file->getBoolValueByName("print_cat_details");
	print_tsdt_details = conf_file->getBoolValueByName("print_tsdt_details");
	print_nit_details = conf_file->getBoolValueByName("print_nit_details");
	print_sdt_details = conf_file->getBoolValueByName("print_sdt_details");
	print_eit_details = conf_file->getBoolValueByName("print_eit_details");
	print_bat_details = conf_file->getBoolValueByName("print_bat_details");
	print_pmt_details = conf_file->getBoolValueByName("print_pmt_details");
	print_pes_video_details = conf_file->getBoolValueByName("print_pes_video_details");
	print_pes_video_details = conf_file->getBoolValueByName("print_pes_video_details");
	print_pes_video_details = conf_file->getBoolValueByName("print_pes_video_details");
	print_pcr_details = conf_file->getBoolValueByName("print_pcr_details");
	print_null_details = conf_file->getBoolValueByName("print_null_details");
	print_unknown_details = conf_file->getBoolValueByName("print_unknown_details");

	delete conf_file;
}
