#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "create_json.h"


cJSON *handle_get_chat_users(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "chat_id")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);
    
    chat_t *chat = NULL;
    
    // Critical resource access: SELECTED CHAT. Start
    bool chat_retrieved_successfully = get_group_data_and_lock_group_mutex(
        call_data, chat_id, &chat
    );

    if (!chat_retrieved_successfully) {
        return create_error_json("No such chat\n");
    }

    cJSON *chat_data_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(chat_data_json, "success", true);
    cJSON_AddNumberToObject(chat_data_json, "chat_id", chat_id);
    cJSON_AddStringToObject(chat_data_json, "chat_name", chat->name);
    cJSON_AddNumberToObject(chat_data_json, "members_num", chat->users_count);
    cJSON *chat_members = cJSON_AddArrayToObject(chat_data_json, "members");

    for (int i = 0; i < chat->users_count; i++) {
        cJSON *member_data_json = cJSON_CreateObject();

        // Critical resource access: CLIENTS HASH TABLE. Start
        pthread_mutex_lock(call_data->general_data->clients_mutex);
        client_t *member_data = ht_get(call_data->general_data->clients, chat->users_id[i]);
        pthread_mutex_unlock(call_data->general_data->clients_mutex);
        // Critical resource access: CLIENTS HASH TABLE. End

        cJSON_AddNumberToObject(member_data_json, "id", chat->users_id[i]);

        // Critical resource access: CLIENT USER DATA. Start
        pthread_mutex_lock(&member_data->user_data->mutex);
        cJSON_AddStringToObject(member_data_json, "login", member_data->user_data->login);
        cJSON_AddStringToObject(member_data_json, "nickname", member_data->user_data->nickname);
        pthread_mutex_unlock(&member_data->user_data->mutex);
        // Critical resource access: CLIENT USER DATA. End


        cJSON_AddItemToArray(chat_members, member_data_json);
    }

    pthread_mutex_unlock(&chat->mutex);
    // Critical resource access: SELECTED CHAT. End

    return chat_data_json;
}

