#ifndef JSON_H
#define JSON_H

#include "block_allocator.h"
#include "frontend.h"

enum json_type
{
	JSON_NULL,
	JSON_OBJECT,
	JSON_ARRAY,
	JSON_STRING,
	JSON_INT,
	JSON_FLOAT,
	JSON_BOOL,
};

struct json_value
{
	json_value *parent;
	json_value *next_sibling;
	json_value *first_child;
	json_value *last_child;

	char *name;
	union
	{
		char *string_value;
		int int_value;
		float float_value;
	};

	json_type type;
};

json_value *json_parse(char *source, char **error_pos, char **error_desc, int *error_line, block_allocator *allocator);

std::string getStrValueFromJSON(json_value *value, std::string label);
int getIntValueFromJSON(json_value *value, std::string label);
bool getBoolValueFromJSON(json_value *value, std::string label);

void parseSyncJSON	(
					json_value *value,
					int *frequency,
					fe_bandwidth_t *bandwidth,
					fe_code_rate_t *code_rate_HP,
					fe_code_rate_t *code_rate_LP,
					fe_spectral_inversion_t *inversion,
					fe_modulation_t *constellation,
					fe_transmit_mode_t *transmission_mode,
					fe_guard_interval_t *guard_interval,
					fe_hierarchy_t *hierarchy_information
				);

#endif
