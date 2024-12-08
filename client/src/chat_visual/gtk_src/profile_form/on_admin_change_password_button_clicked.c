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

    // Disable the button to prevent multiple clicks
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    const gchar *user_login = gtk_editable_get_text(GTK_EDITABLE(GTK_data->profile_data->admin_login_entry));
    const gchar *new_password = gtk_editable_get_text(GTK_EDITABLE(GTK_data->profile_data->admin_password_entry));

    if (user_login == NULL || strlen(user_login) == 0) {
        // Clear success label and its styling
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_success_label), "");
        gtk_widget_remove_css_class(GTK_WIDGET(GTK_data->profile_data->admin_success_label), "success-label");
        
        // Set error message with styling
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_error_label), "Please enter a valid login.");
        gtk_widget_add_css_class(GTK_WIDGET(GTK_data->profile_data->admin_error_label), "error-label");
        
        g_timeout_add(1000, reenable_button_after_delay, button);
        return;
    }

    if (new_password == NULL || strlen(new_password) == 0) {
        // Clear success label and its styling
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_success_label), "");
        gtk_widget_remove_css_class(GTK_WIDGET(GTK_data->profile_data->admin_success_label), "success-label");
        
        // Set error message with styling
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_error_label), "Please enter a valid new password.");
        gtk_widget_add_css_class(GTK_WIDGET(GTK_data->profile_data->admin_error_label), "error-label");
        
        g_timeout_add(1000, reenable_button_after_delay, button);
        return;
    }

    if (GTK_data->call_data == NULL || GTK_data->call_data->ssl == NULL) {
        g_print("Error: SSL is not initialized.\n");
        
        // Clear success label and its styling
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_success_label), "");
        gtk_widget_remove_css_class(GTK_WIDGET(GTK_data->profile_data->admin_success_label), "success-label");
        
        // Set error message with styling
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_error_label), "Internal error: SSL not initialized.");
        gtk_widget_add_css_class(GTK_WIDGET(GTK_data->profile_data->admin_error_label), "error-label");
        
        g_timeout_add(1000, reenable_button_after_delay, button);
        return;
    }

    admin_change_password(GTK_data->call_data->ssl, (char *)user_login, (char *)new_password);

    // Clear error label and its styling
    gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_error_label), "");
    gtk_widget_remove_css_class(GTK_WIDGET(GTK_data->profile_data->admin_error_label), "error-label");
    
    // Set success message with styling
    gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->admin_success_label), "Password successfully changed.");
    gtk_widget_add_css_class(GTK_WIDGET(GTK_data->profile_data->admin_success_label), "success-label");

    // Clear input fields
    gtk_editable_set_text(GTK_EDITABLE(GTK_data->profile_data->admin_login_entry), "");
    gtk_editable_set_text(GTK_EDITABLE(GTK_data->profile_data->admin_password_entry), "");

    // Add timeouts for hiding success message and re-enabling button
    g_timeout_add(1500, hide_label_after_timeout, GTK_data->profile_data->admin_success_label);
    g_timeout_add(1000, reenable_button_after_delay, button);
}
