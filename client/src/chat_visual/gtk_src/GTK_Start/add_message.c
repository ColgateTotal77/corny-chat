#include "GTK.h"

void change_message(GtkWidget *widget, gpointer user_data) {
    (void) widget;

    message_data_t *message_data = (message_data_t*)user_data;

    if (!(*(message_data->is_editing) == false && message_data->own_is_editing == false)) {
        GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(message_data->message_entry));
        const char *message_text = gtk_entry_buffer_get_text(buffer);
        gtk_label_set_text(GTK_LABEL(message_data->message_label), message_text);
        *(message_data->is_editing) = false;
        message_data->own_is_editing = false;

        update_message(message_data->ssl, message_data->message_id, (char *)message_text);

        gtk_entry_buffer_set_text(buffer, "", -1);
    }
    g_signal_handlers_disconnect_by_func(G_OBJECT(message_data->send_button), (gpointer)change_message, message_data);
    g_signal_handlers_disconnect_by_func(G_OBJECT(message_data->message_entry), (gpointer)change_message, message_data);  
}


void on_edit_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    GtkWidget *alignment_box = GTK_WIDGET(user_data);
    message_data_t *message_data = g_object_get_data(G_OBJECT(alignment_box), "message_data");

    g_signal_connect(message_data->send_button, "clicked", G_CALLBACK(change_message), message_data);    
    g_signal_connect(message_data->message_entry, "activate", G_CALLBACK(change_message), message_data);  

    gtk_editable_set_text(GTK_EDITABLE(message_data->message_entry), gtk_label_get_text(GTK_LABEL(message_data->message_label)));
    *(message_data->is_editing) = true;
    message_data->own_is_editing = true;
}

void on_delete_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    GtkWidget *alignment_box = GTK_WIDGET(user_data);
    message_data_t *message_data = g_object_get_data(G_OBJECT(alignment_box), "message_data");

    delete_message(message_data->ssl, message_data->message_id);

    gtk_widget_unparent(alignment_box);
}

void on_message_edit(GtkGestureClick *gesture) {

    GtkWidget *alignment_box = GTK_WIDGET(gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture)));

    GtkWidget *popover = gtk_popover_new();
    
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_popover_set_child(GTK_POPOVER(popover), box);

    GtkWidget *edit_button = gtk_button_new_with_label("Change");
    gtk_box_append(GTK_BOX(box), edit_button);
    g_signal_connect(edit_button, "clicked", G_CALLBACK(on_edit_button_clicked), alignment_box);

    GtkWidget *delete_button = gtk_button_new_with_label("Delete");
    gtk_box_append(GTK_BOX(box), delete_button);
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_button_clicked), alignment_box);
    
    gtk_widget_set_parent(popover, alignment_box);
    
    gtk_popover_popup(GTK_POPOVER(popover));
}

void add_message(GtkWidget *messages_container, const char *message_text, const char *time_text, gboolean is_sent, chat_manager_t *chat_manager, message_data_t *message_data, SSL* ssl) {
    GtkWidget *message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    
    GtkWidget *message_label = gtk_label_new(message_text);
    gtk_widget_add_css_class(message_label, "message-bubble"); 
    
    gtk_label_set_wrap(GTK_LABEL(message_label), TRUE); 

    GtkWidget *time_label = gtk_label_new(time_text);
    gtk_widget_add_css_class(time_label, "message-time");

    // Create a new box for alignment  
    GtkWidget *alignment_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); 
    gtk_box_append(GTK_BOX(alignment_box), message_box);   


    if (is_sent) {
        gtk_box_append(GTK_BOX(message_box), time_label);
        gtk_box_append(GTK_BOX(message_box), message_label);
        gtk_widget_set_halign(alignment_box, GTK_ALIGN_END); // Set the alignment to the right if the message is sent

        message_data->alignment_box = alignment_box;
        message_data->message_label = message_label;
        message_data->time_label = time_label;
        message_data->message_entry = chat_manager->message_entry;
        message_data->is_editing = chat_manager->is_editing;
        message_data->own_is_editing = false;
        message_data->send_button = chat_manager->send_button;
        message_data->ssl = ssl;
        
        g_object_set_data(G_OBJECT(alignment_box), "message_data", message_data);

        GtkGesture *right_click = gtk_gesture_click_new();
        gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(right_click), GDK_BUTTON_SECONDARY);
        g_signal_connect(right_click, "pressed", G_CALLBACK(on_message_edit), NULL);
        gtk_widget_add_controller(alignment_box, GTK_EVENT_CONTROLLER(right_click));
    } 
    else {
        message_data->alignment_box = alignment_box;
        message_data->message_label = message_label;
        gtk_box_append(GTK_BOX(message_box), message_label);
        gtk_box_append(GTK_BOX(message_box), time_label);
        gtk_widget_set_halign(alignment_box, GTK_ALIGN_START); // Set the alignment to the left if the message is sent
    }
    
    // Prepend message_box to the top of the container
    gtk_box_append(GTK_BOX(messages_container), alignment_box);
    
    if(is_sent) {
        scroll_to_bottom(messages_container);
    }
}

