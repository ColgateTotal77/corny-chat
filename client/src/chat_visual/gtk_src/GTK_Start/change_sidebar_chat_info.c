#include "GTK.h"

void change_status_sidebar_chat(chat_data_t *chat, bool is_online) {
    gtk_widget_remove_css_class(chat->status_indicator, is_online ? "status-offline" : "status-online");
    gtk_widget_add_css_class(chat->status_indicator, is_online ? "status-online" : "status-offline");
}

void change_sidebar_chat_info(chat_data_t *chat, char *message, char *time) {
    gtk_label_set_text(GTK_LABEL(chat->time_label), time);
    
    char message_to_sidebar[41];
    if (strlen(message) > 40) {
        
        strncpy(message_to_sidebar, message, 37);
        message_to_sidebar[37] = '.';
        message_to_sidebar[38] = '.';
        message_to_sidebar[39] = '.';
        message_to_sidebar[40] = '\0';
        gtk_label_set_text(GTK_LABEL(chat->message_label), message_to_sidebar);
    }
    else {
        gtk_label_set_text(GTK_LABEL(chat->message_label), message);
    }
}

