#include "create_json.h"
#include "command_codes.h"


cJSON *message_deleted_notification(int message_id, int sender_id, 
                                    bool is_group_message, int group_id) {
    cJSON *notification_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(notification_json, "event_code", DELETED_MESSAGE);
    cJSON_AddNumberToObject(notification_json, "message_type", 
        is_group_message ? GROUP_MESSAGE : PRIVATE_MESSAGE);
    cJSON_AddNumberToObject(notification_json, "message_id", message_id);
    cJSON_AddNumberToObject(notification_json, "sender_id", sender_id);
    cJSON_AddNumberToObject(notification_json, "group_id", group_id);

    return notification_json;
}

static void send_notification_to_group(call_data_t* call_data, cJSON *notif, int group_id) {
    chat_t *chat = NULL;
    
    // Critical resource access: SELECTED CHAT. Start
    bool chat_retrieved_successfully = get_group_data_and_lock_group_mutex(
        call_data, group_id, &chat
    );

    if (!chat_retrieved_successfully) {
        return;
    }

    send_to_group_and_delete_json(call_data, &notif, chat);

    pthread_mutex_unlock(&chat->mutex);
    // Critical resource access: SELECTED CHAT. End
}

static void send_notification_to_client(call_data_t *call_data, cJSON *notif, int client_id) {
    // Critical resource access: CLIENTS HASH TABLE. Start
    pthread_mutex_lock(call_data->general_data->clients_mutex);
    client_t *contact_data = ht_get(call_data->general_data->clients, client_id);
    pthread_mutex_unlock(call_data->general_data->clients_mutex);
    // Critical resource access: CLIENTS HASH TABLE. End

    if (contact_data == NULL) {
        return;
    }

    // Critical resource access: CLIENT USER DATA. Start
    pthread_mutex_lock(&contact_data->user_data->mutex);

    if (!contact_data->user_data->is_active) {
        return;
    }

     pthread_mutex_unlock(&contact_data->user_data->mutex);
    // Critical resource access: CLIENT USER DATA. End

    send_to_client_and_delete_json(&notif, contact_data);
}

cJSON *handle_delete_message(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "message_id")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *message_id_json = cJSON_GetObjectItemCaseSensitive(json, "message_id");
    int message_id = (int)cJSON_GetNumberValue(message_id_json);
    int user_id = call_data->client_data->user_data->user_id;

    s_message message;

    // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
    int rc = get_message_by_id(
        call_data->general_data->db,
        &message, message_id);

    if (rc != SQLITE_OK) {
        pthread_mutex_unlock(call_data->general_data->db_mutex);
        // Critical resource access: DATABASE. Possible end

        cJSON* error_response = create_error_json("Something went wrong 1\n");
        cJSON_AddNumberToObject(error_response, "message_id", message_id);
        return error_response;
    }

    if (message.owner_id != user_id) {
        pthread_mutex_unlock(call_data->general_data->db_mutex);
        // Critical resource access: DATABASE. Possible end

        cJSON* error_response = create_error_json("You have no rights\n");
        cJSON_AddNumberToObject(error_response, "message_id", message_id);
        return error_response;
    }

    rc = delete_message(call_data->general_data->db, message_id);
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End

    if (rc != 1) {
        cJSON* error_response = create_error_json("Something went wrong 2\n");
        cJSON_AddNumberToObject(error_response, "message_id", message_id);
        return error_response;
    }

    bool is_group_message = message.target_group_id != 0;
    
    cJSON *message_deleted_notif = message_deleted_notification(
        message_id, 
        user_id,
        is_group_message,
        message.target_group_id
    );

    if (is_group_message) {
        send_notification_to_group(call_data, message_deleted_notif, message.target_group_id);
    }
    else {
        send_notification_to_client(call_data, message_deleted_notif, message.target_id);
    }
    

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "message_id", message_id);

    return response_json;
}
