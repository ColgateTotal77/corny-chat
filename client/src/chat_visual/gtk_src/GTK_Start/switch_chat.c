#include "GTK.h"

void switch_chat(GtkWidget *widget, chat_manager_t *chat_manager) {    
    if (!GTK_IS_BUTTON(widget) || chat_manager == NULL) {
        printf("ERROR: Invalid widget or chat_manager is NULL\n");
        return;
    }
    
    GHashTableIter iter;
    g_hash_table_iter_init(&iter, chat_manager->chats);
    
    GtkWidget *grid = gtk_button_get_child(GTK_BUTTON(widget));
    GtkWidget *name_box = gtk_grid_get_child_at(GTK_GRID(grid), 1, 0);
    GtkWidget *name_label = gtk_widget_get_first_child(name_box);
    
    const char *contact_name = gtk_label_get_text(GTK_LABEL(name_label));
    
    gpointer chat_id_ptr = g_object_get_data(G_OBJECT(widget), "chat_id");
    int chat_id = GPOINTER_TO_INT(chat_id_ptr);
    // Look up the chat data for this contact
    chat_data_t *new_chat = g_hash_table_lookup(chat_manager->chats, GINT_TO_POINTER(chat_id));
    if (new_chat == NULL || new_chat == chat_manager->active_chat) {
        return;
    }
    
    GtkWidget *current_child = gtk_widget_get_first_child(chat_manager->chat_area_background);

    while (current_child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(current_child);
        gtk_box_remove(GTK_BOX(chat_manager->chat_area_background), current_child);
        current_child = next;
    }
    
    // Update active chat
    chat_manager->active_chat = new_chat;
    
    // Update chat label
    if (GTK_IS_LABEL(chat_manager->chat_user_label)) {
        gtk_label_set_text(GTK_LABEL(chat_manager->chat_user_label), contact_name);
    }
    
    // Add new chat's messages container wrapper to chat area
    if (GTK_IS_WIDGET(new_chat->messages_container_wrapper)) {
        gtk_box_append(GTK_BOX(chat_manager->chat_area_background), new_chat->messages_container_wrapper);
        gtk_widget_set_visible(new_chat->messages_container_wrapper, TRUE);
        gtk_widget_set_visible(new_chat->messages_container, TRUE);
    } 
    else {
        printf("ERROR: New chat's messages container wrapper is not a valid widget\n");
    }
}

