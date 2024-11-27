#include "GTK.h"

// Callback to handle row selection and set the email in the delete input field
void on_login_row_selected(GtkListBox *list_box, GtkListBoxRow *row, gpointer user_data) {
    (void)list_box; // Suppress unused parameter warning

    // Use delete_entry instead of login_entry
    GtkEntry *delete_entry = GTK_ENTRY(user_data);

    if (row) {
        GtkWidget *child = gtk_list_box_row_get_child(row);
        if (GTK_IS_LABEL(child)) {
            const char *login_text = gtk_label_get_text(GTK_LABEL(child));
            gtk_editable_set_text(GTK_EDITABLE(delete_entry), login_text);
        }
    }
}
