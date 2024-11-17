#include "GTK.h"
#include <string.h>

char* on_send_clicked(GtkWidget *messages_container, GtkWidget *message_entry) {
    const char *message_text = gtk_entry_buffer_get_text(
        gtk_entry_get_buffer(GTK_ENTRY(message_entry))
    );
    
    // Only send if the message is not empty
    if (message_text && strlen(message_text) > 0) {
        // Get current time
        GDateTime *now = g_date_time_new_now_local();
        char *time_str = g_date_time_format(now, "%H:%M");
        
        // Add the message to UI
        add_message(messages_container, message_text, time_str, TRUE);
        
        char *message_copy = malloc(strlen(message_text) + 1);
        message_copy =  mx_strdup(message_text);

        if(message_text != NULL) {
            printf("%s \n",message_text);
        }
        
        // Clear the entry
        gtk_entry_buffer_set_text(
            gtk_entry_get_buffer(GTK_ENTRY(message_entry)),
            "",
            0
        );
        
        // Free resources
        g_free(time_str);
        g_date_time_unref(now);

        return message_copy;
    }
    return NULL;
}

