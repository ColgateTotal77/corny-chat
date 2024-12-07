#include "GTK.h"

void on_deactivate_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    GTK_data_t *GTK_data = (GTK_data_t *)user_data;

    // Disable the button to prevent multiple clicks
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    // Get the login from the entry field
    const gchar *user_login = gtk_editable_get_text(GTK_EDITABLE(GTK_data->profile_data->delete_entry));

    if (!user_login || strlen(user_login) == 0) {
        display_ui_message(GTK_data, "Please enter a valid login.", false);
        g_timeout_add(1000, reenable_button_after_delay, button);
        return;
    }

    // Deactivate the user
    deactivate_user(GTK_data->call_data->ssl, (char *)user_login);

    // Display success message
    display_ui_message(GTK_data, "User successfully deactivated!", true);

    // Clear the entry field
    gtk_editable_set_text(GTK_EDITABLE(GTK_data->profile_data->delete_entry), "");

    // Add a 1-second delay before re-enabling the button
    g_timeout_add(1000, reenable_button_after_delay, button);
}
