#include <gtk/gtk.h>

gboolean scroll_idle_callback(gpointer data) {
    GtkAdjustment *adj = GTK_ADJUSTMENT(data);
    // Set the value to the maximum to scroll to bottom
    gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj) - 
                                gtk_adjustment_get_page_size(adj));
    return G_SOURCE_REMOVE;
}

// Scroll to bottom function
void scroll_to_bottom(GtkWidget *container) {
    // Get the parent scrolled window of the messages container
    GtkWidget *scrolled = GTK_WIDGET(gtk_widget_get_ancestor(container, GTK_TYPE_SCROLLED_WINDOW));
    if (scrolled) {
        // Get the vertical adjustment
        GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled));
        // Schedule the scroll after the widget has been allocated and rendered
        g_idle_add(scroll_idle_callback, adj);
    }
}

