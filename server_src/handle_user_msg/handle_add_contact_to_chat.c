#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "create_json.h"


cJSON *handle_add_contact_to_chat(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "contact_id")
        || cJSON_HasObjectItem(json, "chat_id")) {
        return create_error_json("Invalid json format\n");
    }
    
    cJSON *contact_id_json = cJSON_GetObjectItemCaseSensitive(json, "contact_id");
    int contact_id = (int)cJSON_GetNumberValue(contact_id_json);
    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);

    if (!num_inarray(call_data->client_data->user_data->contacts_id, 
                     call_data->client_data->user_data->contacts_count, contact_id)) {
        return create_error_json("No such contact\n");
    }

    chat_t *chat = ht_get(call_data->general_data->chats, chat_id);
    client_t *contact_data = ht_get(call_data->general_data->clients, contact_id);

    if (num_inarray(chat->users_id, chat->users_count, contact_id)) {
        char buffer[BUF_SIZE];
        sprintf(buffer, "%s already joined to chat %s\n", contact_data->user_data->name, chat->name);
        return create_error_json(buffer);
    }

    append_to_intarr(&chat->users_id, &chat->users_count, contact_id);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);

    return response_json;
}

