#include "GTK.h"

void check_remember_me(GtkWidget *remember_me_check, const char *login, const char *password) {

    gboolean remember_me_active = gtk_check_button_get_active(GTK_CHECK_BUTTON(remember_me_check));
    if (remember_me_active) {
        FILE *file = fopen("autoLogin", "w"); 
        fprintf(file, "%s %s", login, password);
        fclose(file);
    } 
    else if (access("autoLogin", F_OK) == 0){
        remove("autoLogin");
    }
}
