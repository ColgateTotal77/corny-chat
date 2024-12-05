#include "create_json.h"
#include "../../libmx/inc/libmx.h"


static chat_t *init_server_group(int chat_id, char *name, int owner_id) {
    chat_t *new_chat = (chat_t*)malloc(sizeof(chat_t));
    new_chat->users_id = (int*)malloc(0);
    new_chat->users_count = 0;
    new_chat->chat_id = chat_id;
    strcpy(new_chat->name, name);
    new_chat->owner_id = owner_id;
    pthread_mutex_init(&new_chat->mutex, NULL);

    append_to_intarr(&new_chat->users_id, &new_chat->users_count, owner_id);

    return new_chat;
}

cJSON *handle_create_chat(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "new_chat_name")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *new_chat_name_json = cJSON_GetObjectItemCaseSensitive(json, "new_chat_name");

    if (strlen(new_chat_name_json->valuestring) < 1) {
        return create_error_json("Invalid json format\n");
    }

    int user_id = call_data->client_data->user_data->user_id;

    // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
    int created_group_id = create_group(
        call_data->general_data->db,
        user_id, new_chat_name_json->valuestring,
        NULL, 0);
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End

	chat_t *new_chat = init_server_group(
        created_group_id, 
        new_chat_name_json->valuestring,
        user_id);
        
    // Critical resource access: USER DATA. Start
    pthread_mutex_lock(&call_data->client_data->user_data->mutex);
    append_to_intarr(&call_data->client_data->user_data->groups_id,
                     &call_data->client_data->user_data->groups_count,
                     created_group_id);
    pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
    // Critical resource access: USER DATA. End
    
    // Critical resource access: CHATS HASH TABLE. Start
    pthread_mutex_lock(call_data->general_data->chats_mutex);
	ht_set(call_data->general_data->chats, new_chat->chat_id, (void*)new_chat);
    pthread_mutex_unlock(call_data->general_data->chats_mutex);
    // Critical resource access: CHATS HASH TABLE. End

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "chat_id", new_chat->chat_id);
    cJSON_AddStringToObject(response_json, "chat_name", new_chat_name_json->valuestring);

    return response_json;
}

