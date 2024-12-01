#include "GTK.h"

void on_admin_change_password_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;

    if (user_data == NULL) {
        g_print("Error: user_data is NULL\n");
        return;
    }

    GTK_data_t *GTK_data = (GTK_data_t *)user_data;

    if (GTK_data->profile_data == NULL) {
        g_print("Error: profile_data is NULL\n");
        return;
    }

    const gchar *user_login = gtk_editable_get_text(GTK_EDITABLE(GTK_data->profile_data->admin_login_entry));
    const gchar *new_password = gtk_editable_get_text(GTK_EDITABLE(GTK_data->profile_data->admin_password_entry));

    if (user_login == NULL || strlen(user_login) == 0) {
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_error_label), "Please enter a valid login.");
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_success_label), ""); // Clear success label
        return;
    }

    if (new_password == NULL || strlen(new_password) == 0) {
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_error_label), "Please enter a valid new password.");
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_success_label), ""); // Clear success label
        return;
    }

    if (GTK_data->call_data == NULL || GTK_data->call_data->ssl == NULL) {
        g_print("Error: SSL is not initialized.\n");
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_error_label), "Internal error: SSL not initialized.");
        return;
    }

    admin_change_password(GTK_data->call_data->ssl, (char *)user_login, (char *)new_password);

    gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_success_label), "Password successfully changed.");
    gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_error_label), ""); // Clear error label
    gtk_editable_set_text(GTK_EDITABLE(GTK_data->profile_data->admin_login_entry), ""); // Clear login entry
    gtk_editable_set_text(GTK_EDITABLE(GTK_data->profile_data->admin_password_entry), ""); // Clear password entry
    g_timeout_add(1500, hide_label_after_timeout, GTK_data->profile_data->admin_success_label);
}
