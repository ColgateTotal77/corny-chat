#include "GTK.h"

void display_ui_message(GTK_data_t *GTK_data, const char *message, bool is_success) {

    if (!GTK_data || !GTK_data->profile_data || !message) {
        fprintf(stderr, "Error: Invalid parameters for display_ui_message\n");
        return;
    }

    if (is_success) {
        if (GTK_data->profile_data->create_success_label) {
            gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->create_success_label), message);
            gtk_widget_add_css_class(GTK_data->profile_data->create_success_label, "success-label");
            g_timeout_add(1500, hide_label_after_timeout, GTK_data->profile_data->create_success_label);
            
            // Clear input fields
            if (GTK_data->profile_data->login_entry) {
                gtk_editable_set_text(GTK_EDITABLE(GTK_data->profile_data->login_entry), "");
            }
            if (GTK_data->profile_data->password_entry) {
                gtk_editable_set_text(GTK_EDITABLE(GTK_data->profile_data->password_entry), "");
            }
        } 
    } else {
        if (GTK_data->profile_data->create_error_label) {
            gtk_label_set_text(GTK_LABEL(GTK_data->profile_data->create_error_label), message);
            gtk_widget_add_css_class(GTK_data->profile_data->create_error_label, "error-label");
            g_timeout_add(1500, hide_label_after_timeout, GTK_data->profile_data->create_error_label);
        }
    }
}
