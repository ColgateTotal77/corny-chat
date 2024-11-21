#include "GTK.h"

void change_sidebar_chat_info(chat_data_t *chat, char *message, char *time) {
    gtk_label_set_text(GTK_LABEL(chat->time_label), time);
    gtk_label_set_text(GTK_LABEL(chat->message_label), message);
}

