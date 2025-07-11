#include "GTK.h"

void reset_all_message_own_is_editing(GHashTable *messages) {
    GList *message_id_list = g_hash_table_get_keys(messages);
    GList *iter;

    for (iter = message_id_list; iter != NULL; iter = iter->next) {
        gpointer key = iter->data;
        message_data_t *message_data = g_hash_table_lookup(messages, key);

        if (message_data != NULL) {
            message_data->own_is_editing = FALSE;
        }
    }
    g_list_free(message_id_list); 
}

void cancel_changing_message(GtkWidget *widget, gpointer user_data) {
    (void) widget;
    message_data_t *message_data = (message_data_t*)user_data;

    gtk_widget_set_visible(message_data->error_label, false);
    gtk_widget_set_visible(g_object_get_data(G_OBJECT(message_data->message_entry), "cancel_button"), false);
    reset_all_message_own_is_editing(message_data->messages);

    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(message_data->message_entry));
    gtk_entry_buffer_set_text(buffer, "", -1);

    g_object_set_data(G_OBJECT(message_data->message_entry), "is_editing", GINT_TO_POINTER(false));
}

void change_message(GtkWidget *widget, gpointer user_data) {
    (void) widget;

    message_data_t *message_data = (message_data_t*)user_data;
    if (*(message_data->this_chat)) {
        if (message_data->own_is_editing) {
            GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(message_data->message_entry));
            const char *message_text = gtk_entry_buffer_get_text(buffer);

            // Check if the message is empty
            if (strlen(message_text) == 0) {
                gtk_label_set_text(GTK_LABEL(message_data->error_label), "Message to chandge should not be 0 lenght!");
                gtk_widget_set_visible(message_data->error_label, true);
            }
            else if(strlen(message_text) >= 500) {
                gtk_label_set_text(GTK_LABEL(message_data->error_label), "Message to chandge should not be more than 500");
                gtk_widget_set_visible(message_data->error_label, true);
            }
            else {
                gtk_label_set_text(GTK_LABEL(message_data->message_label), message_text);
                if (!gtk_widget_get_visible(message_data->edited_label)) {
                    gtk_widget_set_visible(message_data->edited_label, true);
                }

                update_message(message_data->ssl, message_data->message_id, (char *)message_text);

                cancel_changing_message(NULL, (gpointer)message_data);

                gtk_entry_buffer_set_text(buffer, "", -1);

                g_signal_handlers_disconnect_by_func(G_OBJECT(message_data->send_button), (gpointer)change_message, message_data);
                g_signal_handlers_disconnect_by_func(G_OBJECT(message_data->message_entry), (gpointer)change_message, message_data);
            }
        }

    }
}

void on_copy_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *alignment_box = GTK_WIDGET(user_data);
    message_data_t *message_data = g_object_get_data(G_OBJECT(alignment_box), "message_data");
    
    // Get the message text from the label
    const char *message_text = gtk_label_get_text(GTK_LABEL(message_data->message_label));
    
    // Get the clipboard and set its content
    GdkClipboard *clipboard = gtk_widget_get_clipboard(alignment_box);
    gdk_clipboard_set_text(clipboard, message_text);

    // Close the popover
    GtkWidget *popover = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_POPOVER);
    if (GTK_IS_POPOVER(popover)) {
        gtk_popover_popdown(GTK_POPOVER(popover));
    }
}

void on_edit_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *alignment_box = GTK_WIDGET(user_data);
    message_data_t *message_data = g_object_get_data(G_OBJECT(alignment_box), "message_data");

    reset_all_message_own_is_editing(message_data->messages);

    g_signal_connect(message_data->send_button, "clicked", G_CALLBACK(change_message), message_data);
    g_signal_connect(message_data->message_entry, "activate", G_CALLBACK(change_message), message_data);
    g_signal_connect(g_object_get_data(G_OBJECT(message_data->message_entry), "cancel_button"), "clicked", G_CALLBACK(cancel_changing_message), message_data);
    
    gtk_widget_set_visible(g_object_get_data(G_OBJECT(message_data->message_entry), "cancel_button"), true);

    // Set the text in the entry to the current message text
    gtk_editable_set_text(GTK_EDITABLE(message_data->message_entry), gtk_label_get_text(GTK_LABEL(message_data->message_label)));

    // Focus the message entry immediately
    gtk_widget_grab_focus(message_data->message_entry);

    // Set the is_editing state
    g_object_set_data(G_OBJECT(message_data->message_entry), "is_editing", GINT_TO_POINTER(true));
    message_data->own_is_editing = true;

    // Close the popover
    GtkWidget *popover = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_POPOVER);
    if (GTK_IS_POPOVER(popover)) {
        gtk_popover_popdown(GTK_POPOVER(popover));
    }
}


void on_delete_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    GtkWidget *alignment_box = GTK_WIDGET(user_data);
    message_data_t *message_data = g_object_get_data(G_OBJECT(alignment_box), "message_data");

    delete_message(message_data->ssl, message_data->message_id);

    gtk_widget_set_visible(alignment_box, false);
}

void on_message_edit(GtkGestureClick *gesture) {

    GtkWidget *alignment_box = GTK_WIDGET(gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture)));

    GtkWidget *popover = gtk_popover_new();
    gtk_widget_add_css_class(popover, "popover");
    
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_popover_set_child(GTK_POPOVER(popover), box);
    gtk_widget_add_css_class(box, "box-popover");

    GtkWidget *copy_button = gtk_button_new_with_label("Copy");
    gtk_box_append(GTK_BOX(box), copy_button);
    gtk_widget_add_css_class(copy_button, "copy-button-popover");
    g_signal_connect(copy_button, "clicked", G_CALLBACK(on_copy_button_clicked), alignment_box);

    GtkWidget *edit_button = gtk_button_new_with_label("Change");
    gtk_box_append(GTK_BOX(box), edit_button);
    gtk_widget_add_css_class(edit_button, "edit-button-popover");
    g_signal_connect(edit_button, "clicked", G_CALLBACK(on_edit_button_clicked), alignment_box);

    GtkWidget *delete_button = gtk_button_new_with_label("Delete");
    gtk_box_append(GTK_BOX(box), delete_button);
    gtk_widget_add_css_class(delete_button, "delete-button-popover");
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_button_clicked), alignment_box);
    
    gtk_widget_set_parent(popover, alignment_box);
    
    gtk_popover_popup(GTK_POPOVER(popover));
}

void add_message(const char *message_text, const char *time_text, gboolean is_sent, 
                 bool changed, chat_manager_t *manager, SSL* ssl, 
                 int msg_id, chat_data_t *chat, char *nickname) {
    GtkWidget *message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    // Usage example
    GtkWidget *message_label = gtk_label_new(NULL);
    prepare_message_label(message_label, message_text);

    GtkWidget *time_label = gtk_label_new(time_text);
    gtk_widget_add_css_class(time_label, "message-time");

    // Create a new box for alignment  
    GtkWidget *alignment_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(alignment_box, "alignment-bubble");
      
    if (chat->is_group && !is_sent) {
        GtkWidget *nick_label = gtk_label_new(nickname);
        gtk_widget_add_css_class(nick_label, "message-sender");
        gtk_label_set_xalign(GTK_LABEL(nick_label), 0.0);
        gtk_label_set_max_width_chars(GTK_LABEL(nick_label), 15);
        gtk_label_set_ellipsize(GTK_LABEL(nick_label), PANGO_ELLIPSIZE_END); 
        gtk_box_append(GTK_BOX(alignment_box), nick_label);   
    }

    gtk_box_append(GTK_BOX(alignment_box), message_box); 
    GtkWidget *edited_label = gtk_label_new("edited");
    gtk_widget_add_css_class(edited_label, "message-edited");
    
    message_data_t *message_data = create_message_data(msg_id, chat);
    gtk_box_append(GTK_BOX(message_box), time_label);
    gtk_box_append(GTK_BOX(message_box), message_label);

    gtk_box_append(GTK_BOX(alignment_box), edited_label);
    gtk_widget_set_halign(edited_label, GTK_ALIGN_END);

    if (!changed) {
        gtk_widget_set_visible(edited_label, false);
    }

    message_data->alignment_box = alignment_box;
    message_data->message_label = message_label;
    message_data->edited_label = edited_label;
    message_data->error_label = manager->error_label;
    if (is_sent) {
        
        gtk_widget_set_halign(alignment_box, GTK_ALIGN_END); // Set the alignment to the right if the message is sent

        message_data->message_entry = chat->message_entry;
        message_data->own_is_editing = false;
        message_data->send_button = manager->send_button;
        message_data->ssl = ssl;
        message_data->messages = chat->messages;
        message_data->this_chat = &(chat->this_chat);
        gtk_widget_set_halign(edited_label, GTK_ALIGN_END);

        g_object_set_data(G_OBJECT(alignment_box), "message_data", message_data);

        GtkGesture *right_click = gtk_gesture_click_new();
        gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(right_click), GDK_BUTTON_SECONDARY);
        g_signal_connect(right_click, "pressed", G_CALLBACK(on_message_edit), NULL);
        gtk_widget_add_controller(alignment_box, GTK_EVENT_CONTROLLER(right_click));
    } 
    else {
        gtk_widget_set_halign(alignment_box, GTK_ALIGN_START); // Set the alignment to the left if the message is sent
        gtk_widget_set_halign(edited_label, GTK_ALIGN_START);
    }
    
    gtk_box_append(GTK_BOX(chat->messages_container), alignment_box);
    if(is_sent){
        gtk_widget_queue_resize(chat->messages_container);
        scroll_to_bottom(chat->messages_container);
    }
}
