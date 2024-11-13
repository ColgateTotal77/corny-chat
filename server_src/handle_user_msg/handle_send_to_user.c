#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "sql.h"
#include "create_json.h"


cJSON *handle_send_to_user(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "reciever_id")
        || !cJSON_HasObjectItem(json, "message")) {
        return create_error_json("Invalid json format\n");
    }
    cJSON *contact_id_json = cJSON_GetObjectItemCaseSensitive(json, "reciever_id");
    cJSON *message_json = cJSON_GetObjectItemCaseSensitive(json, "message");
    int contact_id = (int)cJSON_GetNumberValue(contact_id_json);

    if (!num_inarray(call_data->client_data->user_data->contacts_id, 
                     call_data->client_data->user_data->contacts_count, contact_id)) {
        return create_error_json("No such contact\n");
    }

    insert_private_message(call_data->general_data->db,
                           call_data->client_data->user_data->user_id,
                           contact_id, message_json->valuestring, NULL);

    char buffer[BUF_SIZE];
    sprintf(buffer, "%s (from private messages): %s\n",
            call_data->client_data->user_data->name,
            message_json->valuestring);
    send_message_to_id(call_data, buffer, contact_id);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);

    return response_json;
}

