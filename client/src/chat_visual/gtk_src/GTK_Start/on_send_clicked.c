#include "GTK.h"
// #include <string.h>

void on_send_clicked(GtkWidget *widget, gpointer user_data) {
    (void) widget;
    GTK_data_t *GTK_data = (GTK_data_t*)user_data; 

    // Check if message_entry is valid
    if (GTK_data->message_entry == NULL) {
        return;
    }
    
    if (GPOINTER_TO_INT(g_object_get_data(G_OBJECT(GTK_data->message_entry), "is_editing"))== true) {
        return;
    }

    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(GTK_data->message_entry));
    const char *message_text = gtk_entry_buffer_get_text(buffer);

    int message_size = strlen(message_text);
    if (message_size > 500) {
        gtk_label_set_text(GTK_LABEL(GTK_data->chat_manager->error_label), "Message to send should not be more than 500");
        gtk_widget_set_visible(GTK_data->chat_manager->error_label, TRUE);
    }
    // Only send if the message is not empty
    else if (message_text && strlen(message_text) > 0) {
        gtk_widget_set_visible(GTK_data->chat_manager->error_label, FALSE);
        GDateTime *now = g_date_time_new_now_local();
        char *time_str = g_date_time_format(now, "%H:%M");
        if (!GTK_data->chat_manager->active_chat->is_group) {
            send_to_user(GTK_data->call_data->ssl, GTK_data->chat_manager->active_chat->contact_id, (char *)message_text);
            
        }
        else {
            send_to_chat(GTK_data->call_data->ssl, GTK_data->chat_manager->active_chat->contact_id, (char *)message_text);
        }
        change_sidebar_chat_info(GTK_data->chat_manager->active_chat, (char *)message_text, time_str);

        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(GTK_data->message_entry)), "", 0);
        g_free(time_str);
        g_date_time_unref(now);
    }
}


