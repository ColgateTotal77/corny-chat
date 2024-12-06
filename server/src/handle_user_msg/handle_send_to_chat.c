#include "server.h"
#include "cJSON.h"
#include "create_json.h"
#include "command_codes.h"

static cJSON *create_incoming_group_message_json(char *message,
                                                 int message_id,
                                                 int sender_id,
                                                 char *sender_nickname,
                                                 int chat_id,
                                                 char *chat_name,
                                                 char *time_string) {
    cJSON *message_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(message_json, "event_code", INCOMING_MESSAGE);
    cJSON_AddNumberToObject(message_json, "message_type", GROUP_MESSAGE);
    cJSON_AddNumberToObject(message_json, "sender_id", sender_id);
    cJSON_AddNumberToObject(message_json, "chat_id", chat_id);
    cJSON_AddNumberToObject(message_json, "message_id", message_id);
    cJSON_AddStringToObject(message_json, "message", message);
    cJSON_AddStringToObject(message_json, "sender_nickname", sender_nickname);
    cJSON_AddStringToObject(message_json, "chat_name", chat_name);
    cJSON_AddStringToObject(message_json, "time", time_string);

    return message_json;
}

cJSON *handle_send_to_chat(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "chat_id")
        || !cJSON_HasObjectItem(json, "message")) {
        return create_error_json("Invalid json format\n");
    }
    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    cJSON *message_json = cJSON_GetObjectItemCaseSensitive(json, "message");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);
    chat_t *chat = NULL;

    // Critical resource access: SELECTED CHAT. Start
    bool chat_retrieved_successfully = get_group_data_and_lock_group_mutex(
        call_data, chat_id, &chat
    );

    if (!chat_retrieved_successfully) {
        cJSON *error_response = create_error_json("No such group\n");
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

   // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
    int message_id = insert_group_message(
        call_data->general_data->db,
        call_data->client_data->user_data->user_id,
        chat_id,
        message_json->valuestring,
        NULL
    );
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End

    if (message_id == -1) {
        pthread_mutex_unlock(&chat->mutex);
        // Critical resource access: SELECTED CHAT. Possible end

        cJSON *error_response = create_error_json("Something went wrong\n");
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    char *time_string = get_string_time();

    // Critical resource access: USER DATA. Start
    pthread_mutex_lock(&call_data->client_data->user_data->mutex);
    cJSON *message_data_json = create_incoming_group_message_json(
        message_json->valuestring,
        message_id,
        call_data->client_data->user_data->user_id,
        call_data->client_data->user_data->nickname,
        chat_id,
        chat->name,
        time_string
    );
    pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
    // Critical resource access: USER DATA. End
    
    send_to_group_and_delete_json(call_data, &message_data_json, chat);
    
    pthread_mutex_unlock(&chat->mutex);
    // Critical resource access: SELECTED CHAT. End

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "chat_id", chat_id);
    cJSON_AddNumberToObject(response_json, "message_id", message_id);
    cJSON_AddStringToObject(response_json, "message", message_json->valuestring);
    cJSON_AddStringToObject(response_json, "time", time_string);

    free(time_string);

    return response_json;
}

