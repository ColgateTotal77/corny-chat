#include "GTK.h"
#include <string.h>

void on_send_clicked(GtkWidget *widget, gpointer user_data) {
    (void) widget;
    GTK_data_t *GTK_data = (GTK_data_t*)user_data;

    // Check if message_entry is valid
    if (GTK_data->message_entry == NULL) {
        printf("message_entry is NULL!\n");
        return;
    }

    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(GTK_data->message_entry));
    const char *message_text = gtk_entry_buffer_get_text(buffer);
    
    // Only send if the message is not empty
    if (message_text && strlen(message_text) > 0) {
        GDateTime *now = g_date_time_new_now_local();
        char *time_str = g_date_time_format(now, "%H:%M");
        
        send_to_user(GTK_data->call_data->ssl, GTK_data->chat_manager->active_chat->contact_id, (char *)message_text);
        add_message(GTK_data->chat_manager->active_chat->messages_container, message_text, time_str, TRUE);
        
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(GTK_data->message_entry)), "", 0);
        g_free(time_str);
        g_date_time_unref(now);
    }
}


