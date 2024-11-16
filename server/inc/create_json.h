#pragma once
#include "server.h"
#include "sql.h"


void add_db_messages_to_json(s_message* db_messages, int messages_num, cJSON **json, char *array_name);
cJSON* json_get_starting_messages(call_data_t *call_data);
cJSON *create_error_json(char *err_msg);
