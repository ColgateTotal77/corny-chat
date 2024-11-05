#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"



void handle_join_chat(call_data_t *call_data, cJSON *json) {
    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);

    int user_id = call_data->client_data->user_data->user_id;

    chat_t *chat = ht_get(call_data->chats, chat_id);

    append_to_intarr(&chat->users_id, &chat->users_count, user_id);

    char buffer[BUF_SIZE];
    sprintf(buffer, "Succesfully joined to chat %s\n", chat->name);
    send_message_to_user(call_data, buffer);
}


