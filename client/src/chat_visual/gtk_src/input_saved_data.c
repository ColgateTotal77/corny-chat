#include "GTK.h"

void input_saved_data(GtkWidget *login_entry, GtkWidget *password_entry, GtkWidget *remember_me_check) {
    if (access("autoLogin", F_OK) == 0) {
        FILE *file = fopen("autoLogin", "r");
        char buffer[60];
        fgets(buffer, sizeof(buffer), file);
        fclose(file);

        char **data = mx_strsplit(buffer, ' ');

        if (data[0] && data[1]) {
            gtk_editable_set_text(GTK_EDITABLE(login_entry), data[0]);
            gtk_editable_set_text(GTK_EDITABLE(password_entry), data[1]);
        }
        gtk_check_button_set_active(GTK_CHECK_BUTTON(remember_me_check), TRUE);
        
        free(data[0]);
        free(data[1]);
        free(data);
    }
}
