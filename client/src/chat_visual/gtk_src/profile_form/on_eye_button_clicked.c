#include "GTK.h"

// Callback to toggle password visibility
void on_eye_button_clicked(GtkToggleButton *button, gpointer user_data) {
    GtkEntry *password_entry = GTK_ENTRY(user_data);
    gboolean active = gtk_toggle_button_get_active(button);

    // Toggle password visibility
    gtk_entry_set_visibility(password_entry, active);

    // Change the icon of the eye button
    if (active) {
        gtk_button_set_icon_name(GTK_BUTTON(button), "view-reveal-symbolic");
    } else {
        gtk_button_set_icon_name(GTK_BUTTON(button), "view-conceal-symbolic");
    }
}
