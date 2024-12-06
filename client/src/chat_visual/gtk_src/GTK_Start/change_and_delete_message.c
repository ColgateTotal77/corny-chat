#include "GTK.h"

void change_message_from_others(chat_data_t *chat, int msg_id, char* new_message) {
    message_data_t *message_data = g_hash_table_lookup(chat->messages , GINT_TO_POINTER(msg_id));
    gtk_label_set_text(GTK_LABEL(message_data->message_label), new_message);

    if (!gtk_widget_get_visible(message_data->edited_label)) {
        gtk_widget_set_visible(message_data->edited_label, true);
    }
}

void delete_message_from_others(chat_data_t *chat, int msg_id) {
    message_data_t *message_data = g_hash_table_lookup(chat->messages , GINT_TO_POINTER(msg_id));
    gtk_widget_set_visible(message_data->alignment_box, false);
}

