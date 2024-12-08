#include "GTK.h"

void on_update_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;

    if (!user_data) {
        fprintf(stderr, "Error: user_data is NULL.\n");
        return;
    }

    GTK_data_t *GTK_data = (GTK_data_t *)user_data;

    if (!GTK_data || !GTK_data->profile_data || !GTK_data->profile_data->nickname_entry || !GTK_data->profile_data->nickname_error_label) {
        fprintf(stderr, "Error: GTK_data or required fields are NULL.\n");
        return;
    }

    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    GtkWidget *nickname_entry = GTK_data->profile_data->nickname_entry;
    GtkWidget *nickname_error_label = GTK_data->profile_data->nickname_error_label;
    GtkWidget *nickname_success_label = GTK_data->profile_data->nickname_success_label;
    const char *new_nickname = gtk_editable_get_text(GTK_EDITABLE(nickname_entry));

    // Validate the nickname
    if (!new_nickname || strlen(new_nickname) < 2 || strlen(new_nickname) > 30) {
        // Clear success label first
        gtk_label_set_text(GTK_LABEL(nickname_success_label), "");
        gtk_widget_remove_css_class(nickname_success_label, "success-label");
        
        // Set error message with proper styling
        gtk_label_set_text(GTK_LABEL(nickname_error_label), "Nickname must be 2-30 characters long.");
        gtk_widget_add_css_class(nickname_error_label, "error-label");
        
        g_timeout_add(1000, reenable_button_after_delay, button);
        g_timeout_add(1500, hide_label_after_timeout, nickname_error_label);
        return;
    }

    // Clear error label first
    gtk_label_set_text(GTK_LABEL(nickname_error_label), "");
    gtk_widget_remove_css_class(nickname_error_label, "error-label");

    // Update the nickname on the server
    update_my_nickname(GTK_data->call_data->ssl, (char *)new_nickname);

    // Update the nickname locally
    update_nickname_in_header(GTK_data, new_nickname);

    // Set success message with proper styling
    gtk_label_set_text(GTK_LABEL(nickname_success_label), "Nickname successfully updated!");
    gtk_widget_add_css_class(nickname_success_label, "success-label");

    // Clear the nickname entry
    gtk_editable_set_text(GTK_EDITABLE(nickname_entry), "");

    // Add timeouts for hiding success message and re-enabling button
    g_timeout_add(1500, hide_label_after_timeout, nickname_success_label);
    g_timeout_add(1000, reenable_button_after_delay, button);
}
