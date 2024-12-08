#pragma once

#include "server.h"

void send_message_to_user(call_data_t *call_data, char *message);
void send_message_to_another_ids(call_data_t *call_data, char *s);
void send_to_user_and_delete_json(call_data_t *call_data, cJSON **json);
void send_to_another_ids_and_delete_json(call_data_t *call_data, cJSON **json);
void send_user_returned_msg(call_data_t *call_data);
void send_user_exit_msg(call_data_t *call_data);
void send_to_client_and_delete_json(cJSON **json, client_t* client_data);
void send_to_group_and_delete_json(call_data_t *call_data, cJSON **json, chat_t *chat);
void send_json_to_client(cJSON **json, client_t* client_data);
void send_to_user_and_delete_json_no_mutexes(call_data_t *call_data, cJSON **json);
void send_to_group_and_delete_json_no_ht_mutex(call_data_t *call_data, 
                                               cJSON **json, chat_t *chat);

