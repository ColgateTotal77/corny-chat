#include "GTK.h"

// Key pressed callback for autocomplete
void on_key_pressed(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data) {
    (void)keycode;
    (void)state;

    if (keyval == GDK_KEY_Tab) {
        GtkEntry *entry = GTK_ENTRY(user_data);
        GtkListBox *list_box = GTK_LIST_BOX(g_object_get_data(G_OBJECT(controller), "list-box"));
        
        if (!entry || !list_box) {
            return;
        }

        const char *current_text = gtk_editable_get_text(GTK_EDITABLE(entry));
        if (!current_text) {
            return;
        }

        const char *suggested_text = find_similar_email(current_text, list_box);
        if (suggested_text) {
            gtk_editable_set_text(GTK_EDITABLE(entry), suggested_text);
            // Set cursor to end of text
            gtk_editable_set_position(GTK_EDITABLE(entry), -1);
        }
    }
}
