#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "sql.h"
#include "create_json.h"
#include "command_codes.h"

static cJSON *create_incoming_private_message_json(char *message, int sender_id, char *sender_nickname) {
    cJSON *message_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(message_json, "event_code", INCOMING_MESSAGE);
    cJSON_AddNumberToObject(message_json, "message_type", PRIVATE_MESSAGE);
    cJSON_AddNumberToObject(message_json, "sender_id", sender_id);
    cJSON_AddStringToObject(message_json, "message", message);
    cJSON_AddStringToObject(message_json, "sender_nickname", sender_nickname);

    return message_json;
}

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

    cJSON *message_data_json = create_incoming_private_message_json(
        message_json->valuestring,
        call_data->client_data->user_data->user_id,
        call_data->client_data->user_data->nickname);
    send_to_id_and_delete_json(call_data, &message_data_json, contact_id);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "contact_id", contact_id);

    return response_json;
}

