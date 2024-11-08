#include "server.h"

void handle_see_all_chats(call_data_t *call_data) {
    char chats_info[BUF_SIZE];
    
    int count = 0;
	entry_t** entries = ht_dump(call_data->general_data->chats, &count);

    int pos = 0;
    
    for (int i = 0; i < count; i++) {
        chat_t* chat_data = entries[i]->value;
        pos += sprintf(&chats_info[pos], "%d.  %s\n", entries[i]->key, chat_data->name);
    }

    free(entries);

    send_message_to_user(call_data, chats_info);
}

