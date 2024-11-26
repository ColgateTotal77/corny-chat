#include "GTK.h"

void change_status_sidebar_chat(chat_data_t *chat, bool is_online) {
    gtk_widget_remove_css_class(chat->status_indicator, is_online ? "status-offline" : "status-online");
    gtk_widget_add_css_class(chat->status_indicator, is_online ? "status-online" : "status-offline");
}

void change_sidebar_chat_info(chat_data_t *chat, char *message, char *time) {
    gtk_label_set_text(GTK_LABEL(chat->time_label), time);
    gtk_label_set_text(GTK_LABEL(chat->message_label), message);
}

