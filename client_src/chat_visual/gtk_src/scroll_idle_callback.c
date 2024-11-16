#include "GTK.h"

// Callback function for idle scrolling
gboolean scroll_idle_callback(gpointer data) {
    GtkAdjustment *adj = GTK_ADJUSTMENT(data);
    // Set the value to the maximum to scroll to bottom
    gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj) - 
                                gtk_adjustment_get_page_size(adj));
    return G_SOURCE_REMOVE;
}
