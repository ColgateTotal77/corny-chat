#include "create_json.h"

static cJSON *init_message_json(s_message db_message) {
    cJSON *message_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(message_json, "msg_id", db_message.id);
    cJSON_AddStringToObject(message_json, "created_at", db_message.createdAt);
    cJSON_AddStringToObject(message_json, "updated_at", db_message.updatedAt);
    cJSON_AddNumberToObject(message_json, "owner_id", db_message.owner_id);
    cJSON_AddNumberToObject(message_json, "target_id", db_message.target_id);
    cJSON_AddNumberToObject(message_json, "target_group_id", db_message.target_group_id);
    cJSON_AddStringToObject(message_json, "message", db_message.message);
    cJSON_AddBoolToObject(message_json, "readed", db_message.readed);

    return message_json;
}

static void convert_db_messages_to_json(call_data_t *call_data, s_message* db_messages, 
                                        int messages_num, cJSON **json, char *array_name) {
    cJSON *messages_json = cJSON_AddArrayToObject(*json, array_name);

    for (int i = 0; i < messages_num; i++) {
        s_message new_message = db_messages[i];

        cJSON *new_message_json = init_message_json(new_message);

        // Critical resource access: CLIENTS HASH TABLE. Start
        pthread_mutex_lock(call_data->general_data->db_mutex);
        client_t *owner_data = ht_get(call_data->general_data->clients, new_message.owner_id);
        pthread_mutex_unlock(call_data->general_data->db_mutex);
        // Critical resource access: CLIENTS HASH TABLE. End

        // Critical resource access: CLIENT USER DATA. Start
        pthread_mutex_lock(&owner_data->user_data->mutex);
        cJSON_AddStringToObject(new_message_json, "nickname", owner_data->user_data->nickname);
        pthread_mutex_unlock(&owner_data->user_data->mutex);
        // Critical resource access: CLIENT USER DATA. End

        cJSON_AddItemToArray(messages_json, new_message_json);
    }

}


cJSON* handle_get_num_of_msgs_from_group(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "group_id")
        || !cJSON_HasObjectItem(json, "before")
        || !cJSON_HasObjectItem(json, "quantity")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *group_id_json = cJSON_GetObjectItemCaseSensitive(json, "group_id");
    cJSON *before_id_json = cJSON_GetObjectItemCaseSensitive(json, "before");
    cJSON *quantity_json = cJSON_GetObjectItemCaseSensitive(json, "quantity");

    chat_t *chat = NULL;
    
    // Critical resource access: SELECTED CHAT. Start
    bool chat_retrieved_successfully = get_group_data_and_lock_group_mutex(
        call_data, group_id_json->valueint, &chat
    );

    if (!chat_retrieved_successfully) {
        cJSON* error_response = create_error_json("Invalid json format\n");
        cJSON_AddNumberToObject(error_response, "group_id", group_id_json->valueint);
        return error_response;
    }

    int num_of_msgs_retrieved = 0;

    // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
    s_message *group_messages = get_new_group_mess(
        call_data->general_data->db,
        group_id_json->valueint,
        quantity_json->valueint,
        &num_of_msgs_retrieved,
        before_id_json->valueint
    );
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End

    pthread_mutex_unlock(&chat->mutex);
    // Critical resource access: SELECTED CHAT. End

    cJSON *response_json = cJSON_CreateObject();
    //cJSON_AddNumberToObject(response_json, "unread_mes_qty",  textings->unread_mes_qty);
    cJSON_AddNumberToObject(response_json, "all_mes_qty",  num_of_msgs_retrieved);
    cJSON_AddNumberToObject(response_json, "group_id", group_id_json->valueint);
    convert_db_messages_to_json(call_data, group_messages, num_of_msgs_retrieved,
                            &response_json, "messages");

    free(group_messages);

    return response_json;
}
