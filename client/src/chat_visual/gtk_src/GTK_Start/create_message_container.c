#include "GTK.h"

GtkWidget* create_message_container(chat_data_t *chat_data) {
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_window, -1, 500);

    chat_data->messages_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), chat_data->messages_container);

    g_object_ref(chat_data->messages_container);
    
    return scrolled_window;
}

