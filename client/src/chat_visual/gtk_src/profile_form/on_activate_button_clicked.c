#include "GTK.h"

void on_activate_button_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    GTK_data_t *GTK_data = (GTK_data_t *)user_data;

    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    const gchar *user_login = gtk_editable_get_text(GTK_EDITABLE(GTK_data->profile_data->activate_entry));

    if (!user_login || strlen(user_login) == 0) {
        // Clear success label first
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->deactivate_success_label), "");
        gtk_widget_remove_css_class(GTK_WIDGET(GTK_data->profile_data->deactivate_success_label), "success-label");
        
        // Set error message with proper styling
        gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->deactivate_error_label), "Please enter a valid login.");
        gtk_widget_add_css_class(GTK_WIDGET(GTK_data->profile_data->deactivate_error_label), "error-label");
        
        g_timeout_add(1000, reenable_button_after_delay, button);
        return;
    }

    activate_user(GTK_data->call_data->ssl, (char *)user_login);

    // Clear error label first
    gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->deactivate_error_label), "");
    gtk_widget_remove_css_class(GTK_WIDGET(GTK_data->profile_data->deactivate_error_label), "error-label");
    
    // Set success message with proper styling
    gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->deactivate_success_label), "User successfully activated.");
    gtk_widget_add_css_class(GTK_WIDGET(GTK_data->profile_data->deactivate_success_label), "success-label");

    gtk_editable_set_text(GTK_EDITABLE(GTK_data->profile_data->activate_entry), "");

    g_timeout_add(1500, hide_label_after_timeout, GTK_data->profile_data->deactivate_success_label);
    g_timeout_add(1000, reenable_button_after_delay, button);
}
