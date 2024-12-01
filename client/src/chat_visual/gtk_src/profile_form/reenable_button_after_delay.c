#include "GTK.h"

gboolean reenable_button_after_delay(gpointer user_data) {
    GtkButton *button = GTK_BUTTON(user_data);
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE); // Re-enable the button
    return G_SOURCE_REMOVE; // Stop the timeout callback
}
