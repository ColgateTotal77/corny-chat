#include "server.h"


void handle_see_my_contacts(call_data_t *call_data) {
    int contacts_count = call_data->client_data->user_data->contacts_count;

    if (contacts_count == 0) {
        send_message_to_user(call_data, "You have no contacts\n");
        return;
    }

    int *contacts_list = call_data->client_data->user_data->contacts_id;

    char users_info[BUF_SIZE];
    char user_name[32];

    int pos = 0;
    
    for (int i = 0; i < contacts_count; i++) {
        client_t *client_data = ht_get(call_data->clients, contacts_list[i]);
        strcpy(user_name, client_data->user_data->name);
        pos += sprintf(&users_info[pos], "%d.  %s\n", contacts_list[i], user_name);
        memset(user_name, 0, sizeof(user_name));
    }

    send_message_to_user(call_data, users_info);
}

