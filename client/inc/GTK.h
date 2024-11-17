#pragma once

#include "client.h"
#include "commands.h"
#include "password.h"
#include <time.h>

typedef struct {
    GtkWidget *messages_container;
    GtkWidget *message_entry;
    char *message; 
    call_data_t *call_data;
    pthread_mutex_t message_mutex;  
} GTK_data_t;

//extern GtkWidget *messages_container;
void add_message(GtkWidget *messages_container, const char *message_text, const char *time_text, gboolean is_sent);
void sleep_ms(int milliseconds);
gboolean scroll_idle_callback(gpointer data);
void scroll_to_bottom(GtkWidget *container);
char *on_send_clicked (GtkWidget *messages_container, GtkWidget *message_entry);
void GTK_start(call_data_t *call_data);
void input_saved_data(GtkWidget *login_entry, GtkWidget *password_entry, GtkWidget *remember_me_check);
void check_remember_me(GtkWidget *remember_me_check, const char *login, const char *password);
void start_login(SSL *ssl);
// Retrieve name and password as a cJSON object for transmission
cJSON* get_name_password(void);

