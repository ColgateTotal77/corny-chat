#include "GTK.h"

chat_data_t* create_chat_data(const char *contact_name, int contact_id, scroll_data_t *scroll_data) {
    //(void)chat_area_background;
    chat_data_t *chat_data = g_new(chat_data_t, 1);

    chat_data->contact_name = g_strdup(contact_name);
    chat_data->contact_id = contact_id;
    
    chat_data->messages_container_wrapper = create_message_container(chat_data);
    gtk_widget_add_css_class(chat_data->messages_container_wrapper, "messages-container");
    // gtk_box_append(GTK_BOX(chat_area_background), chat_data->messages_container_wrapper);

    // Important: Ref the widgets to prevent destruction when removed from parent
    g_object_ref(chat_data->messages_container_wrapper);
    if (scroll_data != NULL) {
        scroll_data->chat = chat_data;
        GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(chat_data->messages_container_wrapper));
        g_signal_connect(adjustment, "value-changed", G_CALLBACK(on_scroll_changed), scroll_data);
    }
    // gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(chat_data->messages_scroll),
    //                             chat_data->messages_container);
    return chat_data;
}

