#include "server.h"

cJSON *handle_see_all_chats(call_data_t *call_data) {
    int count = 0;
	entry_t** entries = ht_dump(call_data->general_data->chats, &count);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "chats_count", count);
    cJSON *chats_json_array = cJSON_AddArrayToObject(response_json, "chats");
    
    for (int i = 0; i < count; i++) {
        chat_t* chat_data = entries[i]->value;
        cJSON *chat_data_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(chat_data_json, "chat_id", entries[i]->key);
        cJSON_AddStringToObject(chat_data_json, "chat_name", chat_data->name);
        cJSON *chat_members = cJSON_AddArrayToObject(chat_data_json, "members");

        for (int i = 0; i < chat_data->users_count; i++) {
            cJSON *member_data_json = cJSON_CreateObject();
            client_t *member_data = ht_get(call_data->general_data->clients, chat_data->users_id[i]);
            cJSON_AddNumberToObject(member_data_json, "id", chat_data->users_id[i]);
            cJSON_AddStringToObject(member_data_json, "login", member_data->user_data->name);
            cJSON_AddItemToArray(chat_members, member_data_json);
        }

        cJSON_AddItemToArray(chats_json_array, chat_data_json);
    }

    free(entries);

    return response_json;
}

