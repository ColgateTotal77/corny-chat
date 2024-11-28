#pragma once

#define _XOPEN_SOURCE

#include "client.h"
#include "commands.h"
#include "password.h"
#include <gtk/gtk.h>

//#include <time.h>

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
    GtkWidget *alignment_box;
    GtkWidget *message_label;
    GtkWidget *time_label;
    //gboolean edited;
    GtkWidget *message_entry;
    gboolean *is_editing;
    gboolean own_is_editing;
    GtkWidget *send_button;

    int message_id;
    SSL* ssl;
} message_data_t;


typedef struct {
    char *contact_name;
    int contact_id;
    int last_message_id;
    GtkWidget *messages_container;
    GtkWidget *messages_container_wrapper;
    GtkWidget *status_indicator;
    GtkWidget *time_label;
    GtkWidget *message_label;
    GtkAdjustment *adjustment;
    GHashTable *messages; 
    bool is_show;
    GtkWidget *button;
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
    gboolean *is_editing;
    GtkWidget *send_button;
    GtkWidget *sidebar;
    GtkWidget *error_label;
    GtkWidget *input_box;
    GtkWidget *chat_header;
    GtkWidget *select_a_chat_label;
} chat_manager_t;

typedef struct {
    GtkListBox *login_list;
    GtkWidget *name_label;
    GtkWidget *nickname_entry;
    GtkWidget *nickname_error_label;
    GtkWidget *password_error_label;
    GtkWidget *password_success_label;
    GtkWidget *new_password_entry_1;
    GtkWidget *new_password_entry_2;
    GtkWidget *old_password_entry;
    GtkWidget *deactivate_entry;
    GtkWidget *deactivate_error_label;
    GtkWidget *create_error_label;  // For create user error messages
    GtkWidget *create_success_label;  // For create user success messages
    GtkWidget *login_entry;         // For login field
    GtkWidget *password_entry;      // For password field
} profile_data_t;

typedef struct {
    //GtkWidget *messages_container;
    char *username;
    int user_id; 
    call_data_t *call_data;
    chat_manager_t *chat_manager;
    user_list_t *user_list;
    profile_data_t *profile_data;
    GtkWidget *window;
    GtkWidget *profile_window;
    GtkWidget *search_bar;
} GTK_data_t;

//extern GtkWidget *messages_container;
void apply_css(GtkWidget *widget, char* path);
void add_message(GtkWidget *messages_container, const char *message_text, const char *time_text, gboolean is_sent, chat_manager_t *chat_manager, message_data_t *message_data, SSL* ssl);
void add_message_to_top(GtkWidget *messages_container, const char *message_text, const char *time_text, gboolean is_sent, GtkAdjustment *adjustment, chat_manager_t *chat_manager, message_data_t *message_data, SSL* ssl);
void sleep_ms(int milliseconds);
void scroll_to_bottom(GtkWidget *container);
void on_send_clicked (GtkWidget *widget, gpointer user_data);
GtkWidget* create_message_container(chat_data_t *chat_data);
GtkWidget* create_chat_item(const char *name, int chat_id, const char *message, const char *time, gboolean is_online, gboolean is_group, chat_manager_t *chat_manager);
chat_data_t* create_chat_data(const char *contact_name, int contact_id, scroll_data_t *scroll_data);
void switch_chat(GtkWidget *widget, chat_manager_t *chat_manager);
void change_sidebar_chat_info(chat_data_t *chat, char *message, char *time);
void change_status_sidebar_chat(chat_data_t *chat, bool is_online);
void on_scroll_changed(GtkAdjustment *adjustment, gpointer user_data);
message_data_t* create_message_data (int message_id, chat_data_t* chat);
void on_message_edit(GtkGestureClick *gesture);
void change_message_from_others(chat_data_t *chat, int msg_id, char* new_message);
void delete_message_from_others(chat_data_t *chat, int msg_id);


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

// profile_form functions
void on_change_password_button_clicked(GtkButton *button, gpointer user_data);
void on_create_button_clicked(GtkButton *button, gpointer user_data);
void on_update_button_clicked(GtkButton *button, gpointer user_data);
void update_nickname_in_header(GTK_data_t *GTK_data, const char *new_nickname);
void on_eye_button_clicked(GtkToggleButton *button, gpointer user_data);
void on_login_row_selected(GtkListBox *list_box, GtkListBoxRow *row, gpointer user_data);
void on_key_pressed(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data);
const char *find_similar_email(const char *input, GtkListBox *list_box);
gboolean hide_label_after_timeout(gpointer label);
char *strdup(const char *str);

