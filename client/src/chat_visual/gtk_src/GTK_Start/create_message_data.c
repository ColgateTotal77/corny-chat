#include "GTK.h"

message_data_t* create_message_data (int message_id, chat_data_t* chat) {
    message_data_t *message_data = g_new(message_data_t, 1);
    message_data->message_id = message_id;
    g_hash_table_insert(chat->messages, GINT_TO_POINTER(message_id), message_data);
    
    return message_data;
}

