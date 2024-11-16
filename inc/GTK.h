#pragma once

#include "client.h"
#include "commands.h"
#include "password.h"
#include <time.h>

//extern GtkWidget *messages_container;
void add_message(GtkWidget *messages_container, const char *message_text, const char *time_text, gboolean is_sent);
void sleep_ms(int milliseconds);
gboolean scroll_idle_callback(gpointer data);
void scroll_to_bottom(GtkWidget *container);
void GTK_start(call_data_t *call_data);
// Function prototypes
void start_login(SSL *ssl);
// Retrieve name and password as a cJSON object for transmission
cJSON* get_name_password(void);

