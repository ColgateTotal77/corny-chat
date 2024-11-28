#include "create_json.h"
#include "command_codes.h"


cJSON *message_updated_notification(int message_id, char *new_message, int sender_id, bool is_group_message, int group_id) {
    cJSON *notification_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(notification_json, "event_code", UPDATED_MESSAGE);
    cJSON_AddNumberToObject(notification_json, "message_type", 
        is_group_message ? GROUP_MESSAGE : PRIVATE_MESSAGE);
    cJSON_AddNumberToObject(notification_json, "message_id", message_id);
    cJSON_AddNumberToObject(notification_json, "sender_id", sender_id);
    cJSON_AddNumberToObject(notification_json, "group_id", group_id);
    cJSON_AddStringToObject(notification_json, "new_message", new_message);

    return notification_json;
}

cJSON *handle_update_message(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "message_id")
        || !cJSON_HasObjectItem(json, "new_message")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *message_id_json = cJSON_GetObjectItemCaseSensitive(json, "message_id");
    cJSON *new_message_json = cJSON_GetObjectItemCaseSensitive(json, "new_message");
    int message_id = (int)cJSON_GetNumberValue(message_id_json);
    int user_id = call_data->client_data->user_data->user_id;

    if (strlen(new_message_json->valuestring) < 1) {
        cJSON* error_response = create_error_json("New message is to short\n");
        cJSON_AddNumberToObject(error_response, "message_id", message_id);
        return error_response;
    }

    s_message message;

    int rc = get_message_by_id(
        call_data->general_data->db,
        &message, message_id);

    if (rc != SQLITE_OK) {
        cJSON* error_response = create_error_json("Something went wrong 1\n");
        cJSON_AddNumberToObject(error_response, "message_id", message_id);
        return error_response;
    }

    if (message.owner_id != user_id) {
        cJSON* error_response = create_error_json("You have no rights\n");
        cJSON_AddNumberToObject(error_response, "message_id", message_id);
        return error_response;
    }

    rc = update_message(call_data->general_data->db, message_id, 
                            new_message_json->valuestring);

    if (rc != 101) {
        cJSON* error_response = create_error_json("Something went wrong 2\n");
        cJSON_AddNumberToObject(error_response, "message_id", message_id);
        return error_response;
    }

    bool is_group_message = message.target_group_id != 0;
    
    cJSON *message_updated_notif = message_updated_notification(
        message_id, 
        new_message_json->valuestring, 
        user_id,
        is_group_message,
        message.target_group_id
    );

    // check for unactive target users
    if (is_group_message) {
        send_to_chat_and_delete_json(call_data, &message_updated_notif, message.target_group_id);
    }
    else {
        send_to_id_and_delete_json(call_data, &message_updated_notif, message.target_id);
    }
    

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "message_id", message_id);

    return response_json;
}
