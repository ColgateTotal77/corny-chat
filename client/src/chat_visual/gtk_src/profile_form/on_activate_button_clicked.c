#include "GTK.h"

void on_activate_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    GTK_data_t *GTK_data = (GTK_data_t *)user_data;

    // Disable the button to prevent multiple clicks
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    // Get the login from the entry field
    const gchar *user_login = gtk_editable_get_text(GTK_EDITABLE(GTK_data->profile_data->activate_entry));

    if (!user_login || strlen(user_login) == 0) {
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->deactivate_error_label), "Please enter a valid login.");
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->deactivate_success_label), "");
        g_timeout_add(1000, reenable_button_after_delay, button); // Add a delay of 1 second before re-enabling
        return;
    }

    // Deactivate the user
    activate_user(GTK_data->call_data->ssl, (char *)user_login);

    // Update UI labels
    gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->deactivate_success_label), "User successfully activated.");
    gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->deactivate_error_label), "");

    // Clear the entry field
    gtk_editable_set_text(GTK_EDITABLE(GTK_data->profile_data->activate_entry), "");

    // Request updated user list from server
    request_user_list(GTK_data->call_data->ssl);

    // Add timeout to hide the success label
    g_timeout_add(1500, hide_label_after_timeout, GTK_data->profile_data->deactivate_success_label);

    // Add a 1-second delay before re-enabling the button
    g_timeout_add(2000, reenable_button_after_delay, button);
}
