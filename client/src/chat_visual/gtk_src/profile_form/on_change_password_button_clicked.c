#include "GTK.h"

void on_change_password_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    GTK_data_t *GTK_data = (GTK_data_t *)user_data;

    // Validate GTK_data and profile_data
    if (!GTK_data || !GTK_data->profile_data) {
        fprintf(stderr, "Error: GTK_data or required fields are NULL.\n");
        return;
    }

    GtkWidget *old_password_entry = GTK_data->profile_data->old_password_entry;
    GtkWidget *new_password_entry_1 = GTK_data->profile_data->new_password_entry_1;
    GtkWidget *new_password_entry_2 = GTK_data->profile_data->new_password_entry_2;
    GtkWidget *password_error_label = GTK_data->profile_data->password_error_label;
    GtkWidget *password_success_label = GTK_data->profile_data->password_success_label;

    const char *old_password = gtk_editable_get_text(GTK_EDITABLE(old_password_entry));
    const char *new_password_1 = gtk_editable_get_text(GTK_EDITABLE(new_password_entry_1));
    const char *new_password_2 = gtk_editable_get_text(GTK_EDITABLE(new_password_entry_2));

    // Validate passwords
    if (!old_password || strlen(old_password) == 0) {
        gtk_label_set_text(GTK_LABEL(password_error_label), "Error: Old password cannot be empty.");
        gtk_widget_add_css_class(password_error_label, "error-label");
        return;
    }

    if (!new_password_1 || strlen(new_password_1) < 8 || strlen(new_password_1) > 20) {
        gtk_label_set_text(GTK_LABEL(password_error_label), "Error: New password must be 8-20 characters long.");
        gtk_widget_add_css_class(password_error_label, "error-label");
        return;
    }

    if (strcmp(new_password_1, new_password_2) != 0) {
        gtk_label_set_text(GTK_LABEL(password_error_label), "Error: New passwords do not match.");
        gtk_widget_add_css_class(password_error_label, "error-label");
        return;
    }

    // Clear error label
    gtk_label_set_text(GTK_LABEL(password_error_label), "");
    gtk_widget_remove_css_class(password_error_label, "error-label");

    // Send change password request
    change_password(GTK_data->call_data->ssl, (char *)old_password, (char *)new_password_1);

    // Simulated response for success (Replace with server response handling)
    bool success = true; // Replace with actual server response

    if (success) {
        gtk_label_set_text(GTK_LABEL(password_success_label), "Password changed successfully.");
        gtk_widget_add_css_class(password_success_label, "success-label");

        // Clear input fields
        gtk_editable_set_text(GTK_EDITABLE(old_password_entry), "");
        gtk_editable_set_text(GTK_EDITABLE(new_password_entry_1), "");
        gtk_editable_set_text(GTK_EDITABLE(new_password_entry_2), "");
    } else {
        gtk_label_set_text(GTK_LABEL(password_error_label), "Error: Incorrect old password.");
        gtk_widget_add_css_class(password_error_label, "error-label");
    }
}
