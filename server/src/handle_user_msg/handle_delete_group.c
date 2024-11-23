#include "create_json.h"
#include "../../libmx/inc/libmx.h"


static void free_server_group(chat_t **chat_data) {
    free((*chat_data)->users_id);
    free(*chat_data);
}

cJSON *handle_delete_group(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "group_id")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *group_id_json = cJSON_GetObjectItemCaseSensitive(json, "group_id");
    int user_id = call_data->client_data->user_data->user_id;

    chat_t *chat_data = ht_get(call_data->general_data->chats, group_id_json->valueint);

    if (NULL == chat_data) {
        return create_error_json("No such group\n");
    }

    if (user_id != chat_data->owner_id) {
        return create_error_json("You have to rights\n");
    }

    int rc = delete_group(call_data->general_data->db, group_id_json->valueint);

    if (rc < 0) {
        return create_error_json("Something went wrong\n");
    }

    for (int i = 0; i < chat_data->users_count; i++) {
        client_t *client_data = ht_get(call_data->general_data->clients, chat_data->users_id[i]);
        remove_from_users_groups(client_data->user_data, chat_data->chat_id);
    }

    ht_del(call_data->general_data->chats, chat_data->chat_id);
    free_server_group(&chat_data);
        

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "chat_id", group_id_json->valueint);

    return response_json;
}

