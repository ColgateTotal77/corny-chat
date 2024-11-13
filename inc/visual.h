#ifndef LOGIN_VISUAL_H
#define LOGIN_VISUAL_H

#include <gtk/gtk.h>
#include <stdbool.h>
#include "cJSON.h"  // Ensure cJSON is included for the cJSON* return type
#include "client.h"

// Function prototypes
void on_activate(GtkApplication *app, gpointer ssl); // Login GUI activation function
// static char* get_name(void);
// static char* get_password(void);
void start_login(GtkApplication *app, SSL *ssl);
// Retrieve name and password as a cJSON object for transmission
cJSON* get_name_password(void);


#endif // LOGIN_VISUAL_H
