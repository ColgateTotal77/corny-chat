#include <gtk/gtk.h>

gboolean scroll_idle_callback(gpointer data) {
    GtkAdjustment *adj = GTK_ADJUSTMENT(data);
    double upper = gtk_adjustment_get_upper(adj);
    double page_size = gtk_adjustment_get_page_size(adj);
    
    gtk_adjustment_set_value(adj, upper - page_size);
    return G_SOURCE_REMOVE;
}

void scroll_to_bottom(GtkWidget *container) {
    if (!container) return;

    GtkWidget *scrolled = gtk_widget_get_ancestor(container, GTK_TYPE_SCROLLED_WINDOW);
    if (scrolled) {
        GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled));
        
        // Force layout update
        gtk_widget_queue_resize(container);
        
        // Add slight delay to ensure layout is complete
        g_timeout_add(100, scroll_idle_callback, adj);
    }
}
