#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include "ConfigFileParser.h"

extern int server_port_no;
extern bool do_not_sync;
extern bool do_not_consume;
extern bool include_ui;
extern bool print_fill_ratio;

extern int prebuffering_as_seconds;
extern int buffer_size_as_seconds;
extern int buffer_lower_threshold_percentage;
extern int buffer_upper_threshold_percentage;
extern bool write_output_to_file_flag;
extern bool write_input_to_file_flag;

extern int video_pid_base;
extern bool video_pid_iterative;
extern std::string video_decoder_ip_base;
extern int video_decoder_port_base;
extern std::string video_secondary_decoder_ip_base;
extern int video_secondary_decoder_port_base;
extern int number_of_primary_streams;

extern std::string video_decoder_protocol;
extern std::string video_renderer_ip;
extern int video_renderer_port;
extern std::string video_renderer_protocol;

extern int audio_pid_base;
extern bool audio_pid_iterative;
extern std::string audio_decoder_ip_base;
extern int audio_decoder_port_base;
extern std::string audio_decoder_protocol;
extern std::string audio_renderer_ip;
extern int audio_renderer_port;
extern std::string audio_secondary_decoder_ip_base;
extern int audio_secondary_decoder_port_base;
extern std::string audio_secondary_decoder_protocol;
extern std::string audio_renderer_protocol;
extern std::string audio_secondary_renderer_ip;
extern int audio_secondary_renderer_port;
extern std::string audio_secondary_renderer_protocol;

extern int private_data_pid_base;
extern bool private_data_pid_iterative;
extern std::string private_data_decoder_ip_base;
extern int private_data_decoder_port_base;
extern std::string private_data_decoder_protocol;

extern std::string private_data_renderer_ip;
extern int private_data_renderer_port;
extern std::string private_data_renderer_protocol;

extern std::string vlc_player_ip;
extern bool add_enhancement_flag;
extern bool send_pcr_flag;

extern bool output_for_vlc_flag;
extern bool no_dvb_flag;
extern int number_of_dvb_receivers;
extern int number_of_p2p_video_base_receivers;
extern int number_of_p2p_video_enhancement_receivers;
extern int number_of_p2p_audio_receivers;
extern int number_of_p2p_private_data_receivers;

extern std::string overlay_client_ip;
extern int overlay_client_port;
extern std::string overlay_client_protocol;
extern bool collaborative_sync_flag;

extern bool print_measurement_values_flag;
extern int print_measurement_values_period_as_seconds;

extern bool file_read_mode_flag;
extern int number_of_file_receivers;
extern std::string stream_file_name;

extern bool additional_content_url_flag;

extern bool no_video_flag;
extern bool no_audio_flag;
extern bool no_private_data_flag;

extern int pts_factor;
extern int pts_divisor;
extern int pts_adder;
extern int pts_extractor;
extern int sync_timeout_as_seconds;
extern int constant_delay;

extern bool ts_parser_mode;
extern int selected_program;
extern bool print_as_hex;
extern bool print_pat_details;
extern bool print_cat_details;
extern bool print_tsdt_details;
extern bool print_nit_details;
extern bool print_sdt_details;
extern bool print_eit_details;
extern bool print_bat_details;
extern bool print_pmt_details;
extern bool print_pes_video_details;
extern bool print_pes_audio_details;
extern bool print_pes_private_data_details;
extern bool print_pcr_details;
extern bool print_null_details;
extern bool print_unknown_details;

void configureAll();

#endif
