#pragma once

#include "client.h"
#include "commands.h"
#include "password.h"
#include <time.h>

typedef struct {
    char *username;
    int id;
    bool is_online;
    char *last_message;
    char *last_time;
} user_data_t;

typedef struct {
    user_data_t **users;
    int count;
    int capacity;
} user_list_t;

typedef struct {
    char *contact_name;
    int contact_id;
    GtkWidget *messages_container;
    GtkWidget *messages_container_wrapper;
} chat_data_t;

typedef struct {
    GHashTable *chats; 
    chat_data_t *active_chat; 
    GtkWidget *chat_area_background;
    GtkWidget *chat_user_label;
} chat_manager_t;

typedef struct {
    //GtkWidget *messages_container;
    GtkWidget *message_entry;
    GtkWidget *sidebar;
    char *message; 
    call_data_t *call_data;
    chat_manager_t *chat_manager;
    user_list_t *user_list;
    bool in_start;
    pthread_mutex_t message_mutex;  
} GTK_data_t;

//extern GtkWidget *messages_container;
void add_message(GtkWidget *messages_container, const char *message_text, const char *time_text, gboolean is_sent);
void sleep_ms(int milliseconds);
gboolean scroll_idle_callback(gpointer data);
void scroll_to_bottom(GtkWidget *container);
void on_send_clicked (GtkWidget *widget, gpointer user_data);
GtkWidget* create_message_container(chat_data_t *chat_data);
GtkWidget* create_chat_item(const char *name, int chat_id, const char *message, const char *time, gboolean is_online, gboolean is_group, chat_manager_t *chat_manager);
chat_data_t* create_chat_data(const char *contact_name, int contact_id);
void switch_chat(GtkWidget *widget, chat_manager_t *chat_manager);

user_list_t* create_user_list(void);
void create_user(user_list_t *list,  char *username, int id, bool is_online, char *last_message,  char *time);
void GTK_start(call_data_t *call_data);
void input_saved_data(GtkWidget *login_entry, GtkWidget *password_entry, GtkWidget *remember_me_check);
void check_remember_me(GtkWidget *remember_me_check, const char *login, const char *password);
void start_login(SSL *ssl);
void profile_start(SSL *ssl);
// Retrieve name and password as a cJSON object for transmission
cJSON* get_name_password(void);

