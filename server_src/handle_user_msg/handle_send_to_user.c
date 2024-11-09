#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"



void handle_send_to_user(call_data_t *call_data, cJSON *json) {
    cJSON *contact_id_json = cJSON_GetObjectItemCaseSensitive(json, "reciever_id");
    cJSON *message_json = cJSON_GetObjectItemCaseSensitive(json, "message");
    int contact_id = (int)cJSON_GetNumberValue(contact_id_json);

    if (!num_inarray(call_data->client_data->user_data->contacts_id, 
                     call_data->client_data->user_data->contacts_count, contact_id)) {
        send_message_to_user(call_data, "No such contact\n");
        return;
    }

    char buffer[BUF_SIZE];
    sprintf(buffer, "%s (from private messages): %s",
            call_data->client_data->user_data->name,
            message_json->valuestring);
    send_message_to_id(call_data, buffer, contact_id);
}

