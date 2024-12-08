#include "create_json.h"

cJSON *handle_see_all_chats(call_data_t *call_data) {
    int count = 0;

    // Critical resource access: CHATS HASH TABLE. Start
    pthread_mutex_lock(call_data->general_data->chats_mutex);
	entry_t** entries = ht_dump(call_data->general_data->chats, &count);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "chats_count", count);
    cJSON *chats_json_array = cJSON_AddArrayToObject(response_json, "chats");
    
    for (int i = 0; i < count; i++) {
        cJSON *chat_data_json = cJSON_CreateObject();

        chat_t* chat_data = entries[i]->value;

        // Critical resource access: SELECTED CHAT. Start
        pthread_mutex_lock(&chat_data->mutex);
        cJSON_AddNumberToObject(chat_data_json, "chat_id", entries[i]->key);
        cJSON_AddStringToObject(chat_data_json, "chat_name", chat_data->name);
        cJSON *chat_members = cJSON_AddArrayToObject(chat_data_json, "members");

        for (int i = 0; i < chat_data->users_count; i++) {
            cJSON *member_data_json = cJSON_CreateObject();

            // Critical resource access: CLIENTS HASH TABLE. Start
            pthread_mutex_lock(call_data->general_data->clients_mutex);
            client_t *member_data = ht_get(call_data->general_data->clients, 
                                           chat_data->users_id[i]);
            pthread_mutex_unlock(call_data->general_data->clients_mutex);
            // Critical resource access: CLIENTS HASH TABLE. End

            // Critical resource access: CLIENT USER DATA. Start
            pthread_mutex_lock(&member_data->user_data->mutex);
            cJSON_AddNumberToObject(member_data_json, "id", chat_data->users_id[i]);
            cJSON_AddStringToObject(member_data_json, "login", member_data->user_data->login);
            pthread_mutex_unlock(&member_data->user_data->mutex);
            // Critical resource access: CLIENT USER DATA. End

            cJSON_AddItemToArray(chat_members, member_data_json);
        }
        pthread_mutex_unlock(&chat_data->mutex);
        // Critical resource access: SELECTED CHAT. End


        cJSON_AddItemToArray(chats_json_array, chat_data_json);
    }
    pthread_mutex_unlock(call_data->general_data->chats_mutex);
    // Critical resource access: CHATS HASH TABLE. End

    free(entries);

    return response_json;
}

