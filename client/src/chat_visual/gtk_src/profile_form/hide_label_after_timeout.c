#include "GTK.h"

gboolean hide_label_after_timeout(gpointer label) {
    gtk_label_set_text(GTK_LABEL(label), ""); // Clear the label text
    gtk_widget_remove_css_class(GTK_WIDGET(label), "error-label");
    gtk_widget_remove_css_class(GTK_WIDGET(label), "success-label");
    return G_SOURCE_REMOVE; // Stop the timeout callback
}
