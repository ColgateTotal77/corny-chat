#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "sql.h"
#include "create_json.h"
#include "command_codes.h"


static cJSON *create_incoming_private_message_json(char *message, int message_id,
                                                   int sender_id, char *sender_nickname,
                                                   char *time_string) {
    cJSON *message_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(message_json, "event_code", INCOMING_MESSAGE);
    cJSON_AddNumberToObject(message_json, "message_type", PRIVATE_MESSAGE);
    cJSON_AddNumberToObject(message_json, "message_id", message_id);
    cJSON_AddNumberToObject(message_json, "sender_id", sender_id);
    cJSON_AddStringToObject(message_json, "message", message);
    cJSON_AddStringToObject(message_json, "sender_nickname", sender_nickname);
    cJSON_AddStringToObject(message_json, "time_reached_server", time_string);

    return message_json;
}

static void update_reciever_and_user_contact_lists(client_t *reciever_data, 
                                                   client_t *client_data) {
    int user_id = client_data->user_data->user_id;
    int **user_list = &client_data->user_data->contacts_id;
    int *user_count = &client_data->user_data->contacts_count;
    int reciever_id = reciever_data->user_data->user_id;
    int **reciever_list = &reciever_data->user_data->contacts_id;
    int *reciever_count = &reciever_data->user_data->contacts_count;

    if (!num_inarray(*reciever_list, *reciever_count, user_id)) {
        append_to_intarr(reciever_list, reciever_count, user_id);
    }

    if (!num_inarray(*user_list, *user_count, reciever_id)) {
        append_to_intarr(user_list, user_count, reciever_id);
    }
}

cJSON *handle_send_to_user(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "reciever_id")
        || !cJSON_HasObjectItem(json, "message")) {
        return create_error_json("Invalid json format\n");
    }
    cJSON *contact_id_json = cJSON_GetObjectItemCaseSensitive(json, "reciever_id");
    cJSON *message_json = cJSON_GetObjectItemCaseSensitive(json, "message");
    int contact_id = (int)cJSON_GetNumberValue(contact_id_json);

    client_t *reciever_data = ht_get(call_data->general_data->clients, contact_id);

    if (reciever_data == NULL) {
        return create_error_json("No such user\n");
    }

    int message_id = insert_private_message(call_data->general_data->db,
                           call_data->client_data->user_data->user_id,
                           contact_id, message_json->valuestring, NULL);
    
    update_reciever_and_user_contact_lists(reciever_data, call_data->client_data);

    char *time_string = get_string_time();

    cJSON *message_data_json = create_incoming_private_message_json(
        message_json->valuestring,
        message_id,
        call_data->client_data->user_data->user_id,
        call_data->client_data->user_data->nickname,
        time_string);
    send_to_id_and_delete_json(call_data, &message_data_json, contact_id);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "contact_id", contact_id);
    cJSON_AddNumberToObject(response_json, "message_id", message_id);
    cJSON_AddStringToObject(response_json, "time_reached_server", time_string);

    free(time_string);

    return response_json;
}

