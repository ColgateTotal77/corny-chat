#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "create_json.h"


cJSON *handle_add_contact_to_chat(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "contact_id")
        || !cJSON_HasObjectItem(json, "chat_id")) {
        return create_error_json("Invalid json format\n");
    }
    
    cJSON *contact_id_json = cJSON_GetObjectItemCaseSensitive(json, "contact_id");
    int contact_id = (int)cJSON_GetNumberValue(contact_id_json);
    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);

    if (!user_has_such_contact(call_data->client_data->user_data, contact_id)) {
        cJSON *error_response = create_error_json("No such contact\n");
        cJSON_AddNumberToObject(error_response, "contact_id", contact_id);
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    chat_t *chat = ht_get(call_data->general_data->chats, chat_id);
    client_t *contact_data = ht_get(call_data->general_data->clients, contact_id);

    if (group_has_such_user(chat, contact_id)) {
        char buffer[BUF_SIZE];
        sprintf(buffer, "%s already joined to chat %s\n", contact_data->user_data->login, chat->name);
        cJSON *error_response = create_error_json(buffer);
        cJSON_AddNumberToObject(error_response, "contact_id", contact_id);
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    const int users[] = {contact_id};
	int res = add_users_to_group(call_data->general_data->db, chat_id, users, 1);
    if (res <= 0) {
        cJSON *error_response = create_error_json("Something went wrong\n");
        cJSON_AddNumberToObject(error_response, "contact_id", contact_id);
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    update_group_users_and_user_groups(chat, contact_data);
    

    cJSON *notification_json = you_were_added_to_group_notification( 
        call_data->client_data->user_data->user_id,
        call_data->client_data->user_data->nickname,
        chat_id,
        chat->name);
    send_to_id_and_delete_json(call_data, &notification_json, contact_id);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "contact_id", contact_id);
    cJSON_AddNumberToObject(response_json, "chat_id", chat_id);

    return response_json;
}

