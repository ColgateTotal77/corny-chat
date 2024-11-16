#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "create_json.h"
#include "command_codes.h"

static void notify_user_about_him_being_add_to_chat(call_data_t *call_data,
                                                    int user_to_notify_id, int contact,
                                                    int chat_id, char *contact_nickname,
                                                    char *chat_name) {
    cJSON *notification_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(notification_json, "event_code", YOU_WERE_ADDED_TO_CHAT);
    cJSON_AddNumberToObject(notification_json, "contact", contact);
    cJSON_AddStringToObject(notification_json, "contact_nickname", contact_nickname);
    cJSON_AddNumberToObject(notification_json, "chat_id", chat_id);
    cJSON_AddStringToObject(notification_json, "chat_name", chat_name);

    send_to_id_and_delete_json(call_data, &notification_json, user_to_notify_id);
}


cJSON *handle_add_contact_to_chat(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "contact_id")
        || !cJSON_HasObjectItem(json, "chat_id")) {
        return create_error_json("Invalid json format\n");
    }
    
    cJSON *contact_id_json = cJSON_GetObjectItemCaseSensitive(json, "contact_id");
    int contact_id = (int)cJSON_GetNumberValue(contact_id_json);
    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);

    if (!num_inarray(call_data->client_data->user_data->contacts_id, 
                     call_data->client_data->user_data->contacts_count, contact_id)) {
        cJSON *error_response = create_error_json("No such contact\n");
        cJSON_AddNumberToObject(error_response, "contact_id", contact_id);
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    chat_t *chat = ht_get(call_data->general_data->chats, chat_id);
    client_t *contact_data = ht_get(call_data->general_data->clients, contact_id);

    if (num_inarray(chat->users_id, chat->users_count, contact_id)) {
        char buffer[BUF_SIZE];
        sprintf(buffer, "%s already joined to chat %s\n", contact_data->user_data->login, chat->name);
        cJSON *error_response = create_error_json("No such contact\n");
        cJSON_AddNumberToObject(error_response, "contact_id", contact_id);
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    append_to_intarr(&chat->users_id, &chat->users_count, contact_id);

    notify_user_about_him_being_add_to_chat(
        call_data,
        contact_id, 
        call_data->client_data->user_data->user_id,
        chat_id,
        call_data->client_data->user_data->nickname,
        chat->name);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "contact_id", contact_id);
    cJSON_AddNumberToObject(response_json, "chat_id", chat_id);

    return response_json;
}

