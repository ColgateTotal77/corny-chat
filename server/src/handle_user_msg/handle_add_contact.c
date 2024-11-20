#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "create_json.h"
#include "command_codes.h"

static void notify_user_about_new_contact(call_data_t *call_data,
                                          int user_to_notify_id, int new_contact_id) {
    cJSON *notification_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(notification_json, "event_code", NEW_CONTACT_WAS_ADDED);
    cJSON_AddNumberToObject(notification_json, "new_contact_id", new_contact_id);
    cJSON_AddStringToObject(notification_json, "nickname", call_data->client_data->user_data->nickname);

    char buffer[BUF_SIZE];
    sprintf(buffer, "%s added you to contacts\n", call_data->client_data->user_data->login);
    cJSON_AddStringToObject(notification_json, "system_message", buffer);

    send_to_id_and_delete_json(call_data, &notification_json, user_to_notify_id);
}

static void add_and_notify_new_contact(call_data_t *call_data, client_t *contact_data) {
    int user_id = call_data->client_data->user_data->user_id;
    int contact_id = contact_data->user_data->user_id;
    int **new_contact_contacts_list = &contact_data->user_data->contacts_id;
    int *new_contact_contacts_count = &contact_data->user_data->contacts_count;

    if (!num_inarray(*new_contact_contacts_list, *new_contact_contacts_count, user_id)) {
        append_to_intarr(new_contact_contacts_list, new_contact_contacts_count, user_id);
        notify_user_about_new_contact(call_data, contact_id, user_id);
    }
}

static bool add_contact_and_notify_user(call_data_t *call_data, client_t *contact_data) {
    int contact_id = contact_data->user_data->user_id;
    int **user_contacts_list = &call_data->client_data->user_data->contacts_id;
    int *user_contacts_count = &call_data->client_data->user_data->contacts_count;

    bool already_was_in_contacts = true;

    if (!num_inarray(*user_contacts_list, *user_contacts_count, contact_id)) {
        append_to_intarr(user_contacts_list, user_contacts_count, contact_id);
        already_was_in_contacts = false;
    }
    
    return already_was_in_contacts;
    
}


cJSON *handle_add_contact(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "new_contact_id")) {
        return create_error_json("Invalid json format\n");
    }
    cJSON *contact_id_json = cJSON_GetObjectItemCaseSensitive(json, "new_contact_id");
    int contact_id = (int)cJSON_GetNumberValue(contact_id_json);

    client_t *contact_data = ht_get(call_data->general_data->clients, contact_id);

    if (contact_data == NULL) {
        char buffer[BUF_SIZE];
        sprintf(buffer, "No user with id %d was found\n", contact_id);
        cJSON *error_response = create_error_json(buffer);
        cJSON_AddNumberToObject(error_response, "new_contact_id", contact_id);
        return error_response;
    }

    add_and_notify_new_contact(call_data, contact_data);

    bool already_was_in_contacts = add_contact_and_notify_user(call_data, contact_data);


    if (already_was_in_contacts) {
        char buffer[BUF_SIZE];
        sprintf(buffer, "%s is already in your contacts\n", contact_data->user_data->login);
        cJSON *error_response = create_error_json(buffer);
        cJSON_AddNumberToObject(error_response, "new_contact_id", contact_id);
        
        return error_response;
    }

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "new_contact_id", contact_id);

    return response_json;
}

