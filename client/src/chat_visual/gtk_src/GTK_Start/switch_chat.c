#include "GTK.h"

void switch_chat(GtkWidget *widget, GTK_data_t *GTK_data) {    
    if (!GTK_IS_BUTTON(widget) || GTK_data == NULL) {
        printf("ERROR: Invalid widget or GTK_data is NULL\n");
        return;
    }
    
    // Determine the chat ID and type
    gpointer chat_id_ptr = g_object_get_data(G_OBJECT(widget), "chat_id");
    int chat_id = GPOINTER_TO_INT(chat_id_ptr);
    
    // Check if it's a group chat
    gboolean is_group = g_object_get_data(G_OBJECT(widget), "is_group") != NULL;
    
    // Determine which chat manager to use
    chat_manager_t *current_manager = is_group ? GTK_data->group_manager : GTK_data->chat_manager;
    
    // Verify current_manager exists
    if (current_manager == NULL) {
        printf("ERROR: Current chat manager is NULL\n");
        return;
    }
    
    // Check if chats hash table exists
    if (current_manager->chats == NULL) {
        printf("ERROR: Chats hash table is NULL\n");
        return;
    }
    
    // Look up the chat data
    chat_data_t *new_chat = g_hash_table_lookup(current_manager->chats, GINT_TO_POINTER(chat_id));
    if (new_chat == NULL) {
        printf("ERROR: Cannot find chat with ID %d in %s manager\n", 
               chat_id, is_group ? "group" : "user");
        return;
    }
    
    // If the chat is already active, do nothing
    if (new_chat == current_manager->active_chat) {
        return;
    }
    
    // Handle initial chat selection
    if (current_manager->active_chat == NULL) {
        gtk_widget_unparent(current_manager->select_a_chat_label);
        current_manager->select_a_chat_label = NULL;
        gtk_widget_set_visible(current_manager->input_box, true);
        gtk_widget_set_visible(current_manager->chat_area_background, true);
        gtk_widget_set_visible(current_manager->chat_header, true);
    }
    
    // Clear existing chat area
    GtkWidget *current_child = gtk_widget_get_first_child(current_manager->chat_area_background);
    while (current_child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(current_child);
        gtk_box_remove(GTK_BOX(current_manager->chat_area_background), current_child);
        current_child = next;
    }
    
    // Update active chat
    current_manager->active_chat = new_chat;
    
    // Get contact name
    GtkWidget *grid = gtk_button_get_child(GTK_BUTTON(widget));
    if (grid == NULL) {
        printf("ERROR: Could not get grid from button\n");
        return;
    }
    
    GtkWidget *name_box = gtk_grid_get_child_at(GTK_GRID(grid), 1, 0);
    if (name_box == NULL) {
        printf("ERROR: Could not get name box from grid\n");
        return;
    }
    
    GtkWidget *name_label = gtk_widget_get_first_child(name_box);
    if (name_label == NULL) {
        printf("ERROR: Could not get name label\n");
        return;
    }
    
    const char *contact_name = gtk_label_get_text(GTK_LABEL(name_label));
    
    // Update chat label
    if (GTK_IS_LABEL(current_manager->chat_user_label)) {
        gtk_label_set_text(GTK_LABEL(current_manager->chat_user_label), contact_name);
    }
    
    // Add new chat's messages container wrapper to chat area
    if (new_chat->messages_container_wrapper == NULL) {
        printf("ERROR: New chat's messages container wrapper is NULL\n");
        return;
    }
    
    if (!GTK_IS_WIDGET(new_chat->messages_container_wrapper)) {
        printf("ERROR: New chat's messages container wrapper is not a valid widget\n");
        return;
    }
    
    gtk_box_append(GTK_BOX(current_manager->chat_area_background), new_chat->messages_container_wrapper);
    gtk_widget_set_visible(new_chat->messages_container_wrapper, TRUE);
    gtk_widget_set_visible(new_chat->messages_container, TRUE);
}

