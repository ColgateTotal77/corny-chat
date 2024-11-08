#pragma once 
#include "server.h"
#include "cJSON.h"

void handle_user_msg(int bytes_received, int *leave_flag, char *client_msg, call_data_t *call_data);
void handle_add_contact_to_chat(call_data_t *call_data, cJSON *json);
void handle_join_chat(call_data_t *call_data, cJSON *json);
void handle_add_contact(call_data_t *call_data, cJSON *json);
void handle_create_chat(call_data_t *call_data, cJSON *json);
void handle_send_to_user(call_data_t *call_data, cJSON *json);
void handle_send_to_chat(call_data_t *call_data, cJSON *json);
void handle_see_all_chats(call_data_t *call_data);
void handle_see_all_users(call_data_t *call_data);
void handle_see_my_contacts(call_data_t *call_data);

