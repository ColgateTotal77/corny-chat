#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"

static void add_and_notify_new_contact(call_data_t *call_data, client_t *contact_data) {
    int user_id = call_data->client_data->user_data->user_id;
    int contact_id = contact_data->user_data->user_id;

    int **new_contact_contacts_list = &contact_data->user_data->contacts_id;
    int *new_contact_contacts_count = &contact_data->user_data->contacts_count;

    if (!num_inarray(*new_contact_contacts_list, *new_contact_contacts_count, user_id)) {
        append_to_intarr(new_contact_contacts_list, new_contact_contacts_count, user_id);
        char buffer[BUF_SIZE];
        sprintf(buffer, "%s added you to contacts\n", call_data->client_data->user_data->name);
        send_message_to_id(call_data, buffer, contact_id);
    }
}

static void add_contact_and_notify_user(call_data_t *call_data, client_t *contact_data) {
    int contact_id = contact_data->user_data->user_id;

    int **user_contacts_list = &call_data->client_data->user_data->contacts_id;
    int *user_contacts_count = &call_data->client_data->user_data->contacts_count;

    if (!num_inarray(*user_contacts_list, *user_contacts_count, contact_id)) {
        append_to_intarr(user_contacts_list, user_contacts_count, contact_id);
        char buffer[BUF_SIZE];
        sprintf(buffer, "You added %s to contacts\n", contact_data->user_data->name);
        send_message_to_user(call_data, buffer);
    }
    else {
        char buffer[BUF_SIZE];
        sprintf(buffer, "%s is already in your contacts\n", contact_data->user_data->name);
        send_message_to_user(call_data, buffer);
    }
}


void handle_add_contact(call_data_t *call_data, cJSON *json) {
    cJSON *contact_id_json = cJSON_GetObjectItemCaseSensitive(json, "new_contact_id");
    int contact_id = (int)cJSON_GetNumberValue(contact_id_json);

    client_t *contact_data = ht_get(call_data->general_data->clients, contact_id);

    if (contact_data == NULL) {
        char buffer[BUF_SIZE];
        sprintf(buffer, "No user with id %d was found\n", contact_id);
        send_message_to_user(call_data, buffer);
        return;
    }

    add_and_notify_new_contact(call_data, contact_data);

    add_contact_and_notify_user(call_data, contact_data);
}

