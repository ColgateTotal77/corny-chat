#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"

void handle_create_chat(call_data_t *call_data, cJSON *json) {
    cJSON *new_chat_name_json = cJSON_GetObjectItemCaseSensitive(json, "new_chat_name");
    int user_id = call_data->client_data->user_data->user_id;

	chat_t *new_chat = (chat_t*)malloc(sizeof(chat_t));
    new_chat->users_id = (int*)malloc(0);
    new_chat->users_count = 0;
    new_chat->chat_id = *(call_data->general_data->chat_uid);
    *(call_data->general_data->chat_uid) = *(call_data->general_data->chat_uid) + 1;
    strcpy(new_chat->name, new_chat_name_json->valuestring);

    append_to_intarr(&new_chat->users_id, &new_chat->users_count, user_id);
	ht_set(call_data->general_data->chats, new_chat->chat_id, (void*)new_chat);
}

