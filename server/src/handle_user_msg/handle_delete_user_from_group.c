#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "create_json.h"
#include "command_codes.h"

static cJSON *notif_you_were_deleted_from_group(int chat_id, char* chat_name) {
    cJSON *notification_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(notification_json, "event_code", YOU_WERE_DELETED_FROM_GROUP);
    cJSON_AddNumberToObject(notification_json, "chat_id", chat_id);
    cJSON_AddStringToObject(notification_json, "chat_name", chat_name);

    return notification_json;
}


cJSON *handle_delete_user_from_group(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "user_id")
        || !cJSON_HasObjectItem(json, "chat_id")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *user_to_delete_id_json = cJSON_GetObjectItemCaseSensitive(json, "user_id");
    int user_to_delete_id = (int)cJSON_GetNumberValue(user_to_delete_id_json);
    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);

    chat_t *chat = ht_get(call_data->general_data->chats, chat_id);

    if (chat == NULL 
        || !group_has_such_user(chat, call_data->client_data->user_data->user_id)) {
        cJSON *error_response = create_error_json("No such group\n");
        cJSON_AddNumberToObject(error_response, "user_id", user_to_delete_id);
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    if (!is_user_group_owner(call_data, chat_id)) {
        return create_error_json("You have to rights\n");
    }

    client_t *contact_data = ht_get(call_data->general_data->clients, user_to_delete_id);

    if (contact_data == NULL) {
        cJSON *error_response = create_error_json("No such user exists\n");
        cJSON_AddNumberToObject(error_response, "user_id", user_to_delete_id);
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    if (!group_has_such_user(chat, user_to_delete_id)) {
        cJSON *error_response = create_error_json("No such group user\n");
        cJSON_AddNumberToObject(error_response, "user_id", user_to_delete_id);
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    remove_from_group_users(chat, user_to_delete_id);
    remove_from_users_groups(contact_data->user_data, chat_id);

    cJSON *notification_json = notif_you_were_deleted_from_group(chat_id, chat->name);
    send_to_id_and_delete_json(call_data, &notification_json, user_to_delete_id);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "user_id", user_to_delete_id);
    cJSON_AddNumberToObject(response_json, "chat_id", chat_id);

    return response_json;
}

