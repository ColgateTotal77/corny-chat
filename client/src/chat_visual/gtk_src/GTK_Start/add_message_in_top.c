#include "GTK.h"

void add_message_to_top(const char *message_text, const char *time_text, gboolean is_sent, bool changed, chat_manager_t *manager, SSL* ssl, int msg_id, chat_data_t *chat, char *nicknake) {
    GtkWidget *message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    //gtk_widget_add_css_class(message_box, "message-bubble");

    GtkWidget *message_label = gtk_label_new(message_text); 
    prepare_message_label(message_label, message_text);

    GtkWidget *time_label = gtk_label_new(time_text);
    gtk_widget_add_css_class(time_label, "message-time");

    // Create a new box for alignment  
    GtkWidget *alignment_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(alignment_box, "alignment-bubble"); 
      
    if (chat->is_group && !is_sent) {
        GtkWidget *nick_label = gtk_label_new(nicknake);
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
    
//Пізніше дороблю!
    // gdouble value = gtk_adjustment_get_value(chat->adjustment); // Поточне становище скролла
    //gdouble upper = gtk_adjustment_get_upper(adjustment); // Максимальне становище скролла
    //gdouble page_size = gtk_adjustment_get_page_size(chat->adjustment); // Висота видимої області

    gtk_box_prepend(GTK_BOX(chat->messages_container), alignment_box);

    // gtk_adjustment_set_value(chat->adjustment, value);
}

