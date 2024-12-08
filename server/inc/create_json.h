#pragma once
#include "server.h"
#include "sql.h"


void add_db_messages_to_json(s_message* db_messages, 
                             int messages_num, cJSON **json, char *array_name);
