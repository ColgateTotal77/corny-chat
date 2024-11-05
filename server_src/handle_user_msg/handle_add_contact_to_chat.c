#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"



void handle_add_contact_to_chat(call_data_t *call_data, cJSON *json) {
    cJSON *contact_id_json = cJSON_GetObjectItemCaseSensitive(json, "contact_id");
    int contact_id = (int)cJSON_GetNumberValue(contact_id_json);
    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);

    if (!num_inarray(call_data->client_data->user_data->contacts_id, 
                     call_data->client_data->user_data->contacts_count, contact_id)) {
        send_message_to_user(call_data, "No such contact\n");
        return;
    }

    chat_t *chat = ht_get(call_data->chats, chat_id);
    client_t *contact_data = ht_get(call_data->clients, contact_id);

    if (num_inarray(chat->users_id, chat->users_count, contact_id)) {
        char buffer[BUF_SIZE];
        sprintf(buffer, "%s already joined to chat %s\n", contact_data->user_data->name, chat->name);
        send_message_to_user(call_data, buffer);
        return;
    }

    append_to_intarr(&chat->users_id, &chat->users_count, contact_id);

    char buffer[BUF_SIZE];
    sprintf(buffer, "Succesfully added %s to chat %s\n", contact_data->user_data->name, chat->name);
    send_message_to_user(call_data, buffer);
}

