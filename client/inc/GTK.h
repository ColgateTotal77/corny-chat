#pragma once

#define _XOPEN_SOURCE

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
    GtkWidget *status_indicator;
    GtkWidget *time_label;
    GtkWidget *message_label;
} chat_data_t;

typedef struct {
    chat_data_t *chat;
    SSL* ssl;
} scroll_data_t;

typedef struct {
    GHashTable *chats; 
    chat_data_t *active_chat; 
    GtkWidget *chat_area_background;
    GtkWidget *chat_user_label;
    GtkWidget *message_entry;
    GtkWidget *sidebar;
} chat_manager_t;

typedef struct {
    GtkListBox *login_list;
} profile_data_t;

typedef struct {
    //GtkWidget *messages_container;
    char *message; 
    call_data_t *call_data;
    chat_manager_t *chat_manager;
    user_list_t *user_list;
    profile_data_t *profile_data;
    GtkWidget *window;
    GtkWidget *profile_window;
} GTK_data_t;

//extern GtkWidget *messages_container;
void add_message(GtkWidget *messages_container, const char *message_text, const char *time_text, gboolean is_sent);
void sleep_ms(int milliseconds);
gboolean scroll_idle_callback(gpointer data);
void scroll_to_bottom(GtkWidget *container);
void on_send_clicked (GtkWidget *widget, gpointer user_data);
GtkWidget* create_message_container(chat_data_t *chat_data);
GtkWidget* create_chat_item(const char *name, int chat_id, const char *message, const char *time, gboolean is_online, gboolean is_group, chat_manager_t *chat_manager);
chat_data_t* create_chat_data(const char *contact_name, int contact_id, scroll_data_t *scroll_data);
void switch_chat(GtkWidget *widget, chat_manager_t *chat_manager);
void change_sidebar_chat_info(chat_data_t *chat, char *message, char *time);
void change_status_sidebar_chat(chat_data_t *chat, bool is_online);
void on_scroll_changed(GtkAdjustment *adjustment, gpointer user_data);

user_list_t* create_user_list(void);
void create_user(user_list_t *list,  char *username, int id, bool is_online, char *last_message,  char *time);
void GTK_start(call_data_t *call_data);
void input_saved_data(GtkWidget *login_entry, GtkWidget *password_entry, GtkWidget *remember_me_check);
void check_remember_me(GtkWidget *remember_me_check, const char *login, const char *password);
void start_login(SSL *ssl);
void profile_start(GTK_data_t *GTK_data);
void profile_css(GtkWidget *widget);
void update_login_list(GtkListBox *login_list, cJSON *parsed_json);
// Retrieve name and password as a cJSON object for transmission
cJSON* get_name_password(void);

