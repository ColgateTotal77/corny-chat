#include <gtk/gtk.h>

void add_message_to_top(GtkWidget *messages_container, const char *message_text, const char *time_text, gboolean is_sent, GtkAdjustment *adjustment) {
    (void)adjustment;
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
    } 
    else {
        gtk_box_append(GTK_BOX(message_box), message_label);
        gtk_box_append(GTK_BOX(message_box), time_label);
        gtk_widget_set_halign(alignment_box, GTK_ALIGN_START); // Set the alignment to the left if the message is sent
    }

//Пізніше дороблю!
    // gdouble value = gtk_adjustment_get_value(adjustment); // Поточне становище скролла
    // gdouble upper = gtk_adjustment_get_upper(adjustment); // Максимальне становище скролла
    // gdouble page_size = gtk_adjustment_get_page_size(adjustment); // Висота видимої області

    gtk_box_prepend(GTK_BOX(messages_container), alignment_box);

    // gtk_adjustment_set_value(adjustment, value + page_size);
}

