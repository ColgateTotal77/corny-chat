#include "server.h"

cJSON *handle_see_all_users(call_data_t *call_data) {
    int count = 0;
	entry_t** entries = ht_dump(call_data->general_data->clients, &count);
    entry_t* entry;
    client_t *client_data;

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "users_count", count);
    cJSON *users_json_array = cJSON_AddArrayToObject(response_json, "users");
    
    for (int i = 0; i < count; i++) {
        entry = entries[i];
        client_data = (client_t*)entry->value;

        cJSON *user_data_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(user_data_json, "id", entry->key);
        cJSON_AddStringToObject(user_data_json, "login", client_data->user_data->login);
        cJSON_AddStringToObject(user_data_json, "nickname", client_data->user_data->nickname);
        cJSON_AddBoolToObject(user_data_json, "online", client_data->user_data->is_online);
        cJSON_AddBoolToObject(user_data_json, "is_admin", true);

        cJSON_AddItemToArray(users_json_array, user_data_json);
    }
    
    free(entries);

    return response_json;
}

