#include "GTK.h"

void switch_chat(GtkWidget *widget, GTK_data_t *GTK_data) {
    if (!GTK_IS_BUTTON(widget) || GTK_data == NULL) {
        printf("ERROR: Invalid widget or GTK_data is NULL\n");
        return;
    }

    // Determine the chat ID and type
    gpointer chat_id_ptr = g_object_get_data(G_OBJECT(widget), "chat_id");
    int chat_id = GPOINTER_TO_INT(chat_id_ptr);
    gboolean is_group = g_object_get_data(G_OBJECT(widget), "is_group") != NULL;    

    chat_manager_t *current_manager = is_group ? GTK_data->group_manager : GTK_data->chat_manager;
    chat_manager_t *other_manager = !is_group ? GTK_data->group_manager : GTK_data->chat_manager;

    // // Check if current_manager is NULL
    // if (current_manager == NULL) {
    //     printf("ERROR: Current manager is NULL\n");
    //     return;
    // }

    if(current_manager->active_chat == NULL) {
        if(current_manager->select_a_chat_label && other_manager->select_a_chat_label){
            // gtk_widget_unparent(current_manager->select_a_chat_label); // відв'язили
            // current_manager->select_a_chat_label = NULL;
            // other_manager->select_a_chat_label = NULL;
            gtk_widget_set_visible(current_manager->select_a_chat_label, false);
        } else{
            printf("ERROR: current_manager->select_a_chat_label && other_manager->select_a_chat_label not available\n");
        }
        
        if(current_manager->input_box){
            gtk_widget_set_visible(current_manager->input_box, true);
        }else{
            printf("ERROR: current_manager->input_box not available\n");
        }
        if(current_manager->chat_area_background){
            gtk_widget_set_visible(current_manager->chat_area_background, true);
        }else{
            printf("ERROR: current_manager->chat_area_background not available\n");
        }
        if(current_manager->chat_header){
            gtk_widget_set_visible(current_manager->chat_header, true);
        } else{
            printf("ERROR: current_manager->chat_header not available\n");
        }
    }
    
    chat_data_t *new_chat = g_hash_table_lookup(current_manager->chats, GINT_TO_POINTER(chat_id));

    // If new_chat is NULL, print more details
    if (new_chat == NULL) {
        printf("ERROR: Chat lookup returned NULL\n");
        return;
    }
    
    if (new_chat->messages_container_wrapper == NULL || !GTK_IS_WIDGET(new_chat->messages_container_wrapper)) {
        printf("ERROR: messages_container_wrapper for Chat ID %d is invalid\n", chat_id);
        return;
    }

    if (new_chat->messages_container == NULL || !GTK_IS_WIDGET(new_chat->messages_container)) {
        printf("ERROR: messages_container for Chat ID %d is invalid\n", chat_id);
        return;
    }

    // Check if the new chat is already active, considering the context
    if (current_manager->active_chat != NULL 
        && current_manager->active_chat->contact_id == chat_id 
        && current_manager->active_chat->is_group == is_group) {
        g_print("-DEBUG: Chat ID %d is already active in the correct context\n", chat_id);
        return;
    }
    else if (current_manager->active_chat != NULL) {
        current_manager->active_chat->this_chat = false;
    }

    // g_print("DEBUG: select_a_chat_label = %p\n", (void *)current_manager->select_a_chat_label);

    // if (current_manager->select_a_chat_label != NULL) {
    //     g_print("DEBUG: select_a_chat_label is not NULL\n");
    //     if (!GTK_IS_WIDGET(current_manager->select_a_chat_label)) {
    //         printf("ERROR: select_a_chat_label is not a valid GTK widget\n");
    //         return;
    //     }
    // }

    // // Handle initial chat setup
    // if (GTK_IS_WIDGET(current_manager->select_a_chat_label)) {
    //     g_print("DEBUG: select_a_chat_label is a valid widget\n");
    //     gtk_widget_unparent(current_manager->select_a_chat_label);
    //     current_manager->select_a_chat_label = NULL;
    // } else {
    //     printf("ERROR: select_a_chat_label is not a valid widget\n");
    // }

    if (current_manager->input_box && GTK_IS_WIDGET(current_manager->input_box)) {
        // printf("2s\n");
        gtk_widget_set_visible(current_manager->input_box, true);
    }
    if (current_manager->chat_area_background && GTK_IS_WIDGET(current_manager->chat_area_background)) {
        // printf("3s\n");
        gtk_widget_set_visible(current_manager->chat_area_background, true);
    }
    if (current_manager->chat_header && GTK_IS_WIDGET(current_manager->chat_header)) {
        // printf("4s\n");
        gtk_widget_set_visible(current_manager->chat_header, true);
    }

    // Clear existing chat area
    if (current_manager->chat_area_background) {
        GtkWidget *current_child = gtk_widget_get_first_child(current_manager->chat_area_background);
        while (current_child != NULL) {
            GtkWidget *next = gtk_widget_get_next_sibling(current_child);
            gtk_box_remove(GTK_BOX(current_manager->chat_area_background), current_child);
            current_child = next;
        }
    } else {
        printf("ERROR: Chat area background is NULL\n");
        return;
    }

    // Update active chat
    current_manager->active_chat = new_chat;
    other_manager->active_chat = new_chat;
    new_chat->this_chat = true;
    // Extract contact name
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
    GtkWidget *chat_user_label = is_group ? 
        GTK_data->group_manager->chat_user_label : 
        GTK_data->chat_manager->chat_user_label;

    if (GTK_IS_LABEL(chat_user_label)) {
        gtk_label_set_text(GTK_LABEL(chat_user_label), contact_name);
    } else {
        printf("ERROR: Chat user label is not a valid GTK label\n");
    }

    if(is_group){
        gtk_widget_set_visible(GTK_data->chat_manager->avatar_circle, FALSE);
        gtk_widget_set_visible(GTK_data->group_manager->avatar_circle, TRUE);
        gtk_widget_set_visible(GTK_data->group_manager->settings_group_button, TRUE);
    } else{
        const char *image_path = current_manager->active_chat->is_active ? "src/chat_visual/images/person.svg" : "src/chat_visual/images/RIP.svg";
        gtk_image_set_from_file(GTK_IMAGE(GTK_data->chat_manager->avatar_circle), image_path);
        gtk_widget_set_visible(GTK_data->group_manager->avatar_circle, FALSE);
        gtk_widget_set_visible(GTK_data->group_manager->settings_group_button, FALSE);
        gtk_widget_set_visible(GTK_data->chat_manager->avatar_circle, TRUE);
    }

    if (GTK_IS_LABEL(chat_user_label)) {
        gtk_label_set_text(GTK_LABEL(chat_user_label), contact_name);
    } else {
        printf("ERROR: Chat user label is not a valid GTK label\n");
    }

    // Add new chat's messages container wrapper
    if (new_chat->messages_container_wrapper == NULL) {
        printf("ERROR: New chat's messages_container_wrapper is NULL\n");
        return;
    }

    if (!GTK_IS_WIDGET(new_chat->messages_container_wrapper)) {
        printf("ERROR: New chat's messages_container_wrapper is not a valid widget\n");
        return;
    }
    if(GPOINTER_TO_INT(g_object_get_data(G_OBJECT(GTK_data->message_entry), "is_editing"))== true) {
        gtk_widget_set_visible(g_object_get_data(G_OBJECT(GTK_data->message_entry), "cancel_button"), false);
    }

    gtk_widget_set_visible(GTK_data->message_entry, false);
    GTK_data->message_entry = new_chat->message_entry;
    
    if (!gtk_widget_get_parent(GTK_data->message_entry)) {
        gtk_overlay_set_child(GTK_OVERLAY(GTK_data->entry_overlay), GTK_data->message_entry);
        g_signal_connect(GTK_data->message_entry, "activate", G_CALLBACK(on_send_clicked), GTK_data);
    }
    
    if(GPOINTER_TO_INT(g_object_get_data(G_OBJECT(GTK_data->message_entry), "is_editing"))== true) {
        gtk_widget_set_visible(g_object_get_data(G_OBJECT(GTK_data->message_entry), "cancel_button"), true);
    }

    gtk_widget_set_visible(GTK_data->message_entry, true);
    gtk_widget_grab_focus(GTK_data->message_entry);

    if(new_chat->is_group) {
        GTK_data->group_manager->user_list_for_add = GTK_WIDGET(new_chat->user_list_for_add);
        GTK_data->group_manager->user_list_for_delete = GTK_WIDGET(new_chat->user_list_for_delete);
    }

    gtk_box_append(GTK_BOX(current_manager->chat_area_background), new_chat->messages_container_wrapper);
    gtk_widget_set_visible(new_chat->messages_container_wrapper, TRUE);
    gtk_widget_set_visible(new_chat->messages_container, TRUE);
}
