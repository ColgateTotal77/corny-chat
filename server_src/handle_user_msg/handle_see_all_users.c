#include "server.h"

void handle_see_all_users(call_data_t *call_data) {
    char users_info[BUF_SIZE];
    char user_name[32];
    int count = 0;
	entry_t** entries = ht_dump(call_data->general_data->clients, &count);
    entry_t* entry;
    client_t *client_data;

    int pos = 0;
    
    for (int i = 0; i < count; i++) {
        entry = entries[i];
        client_data = (client_t*)entry->value;
        strcpy(user_name, client_data->user_data->name);
        pos += sprintf(&users_info[pos], "%d.  %s. %s\n", entry->key, user_name,
                       client_data->user_data->is_online?"*online":"offline");
        
        memset(user_name, 0, sizeof(user_name));
        
        
    }
    
    free(entries);

    send_message_to_user(call_data, users_info);
}

