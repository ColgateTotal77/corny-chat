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
    GtkWidget *message_entry;
    gboolean own_is_editing;
    GtkWidget *send_button;
    GHashTable *messages; 
    GtkWidget *edited_label;
    GtkWidget *cancel_button;
    int message_id;
    SSL* ssl;
    gboolean *this_chat;
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
    GtkWidget *number_of_unread_messages;
    bool is_group;
    GtkWidget *avatar_circle;
    bool is_active;

    GtkListBox *user_list_for_add;
    GtkListBox *user_list_for_delete;
    GtkWidget *message_entry;
    gboolean this_chat;
} chat_data_t;

typedef struct {
    chat_data_t *chat;
    SSL *ssl;
} scroll_data_t;

typedef struct {
    GHashTable *chats; 
    chat_data_t *active_chat; 
    GtkWidget *chat_area_background;
    GtkWidget *chat_user_label;
    GtkWidget *message_entry;
    GtkWidget *send_button;
    GtkWidget *sidebar;
    GtkWidget *error_label;
    GtkWidget *input_box;
    GtkWidget *chat_header;
    GtkWidget *select_a_chat_label;
    GtkWidget *cancel_button;
    GtkWidget *avatar_circle;
    GtkWidget *settings_group_button;

    GtkWidget *user_list_for_add;
    GtkWidget *user_list_for_delete;
} chat_manager_t;

typedef struct {
    GtkStack *carousel_stack;
    GtkListBox *login_list;
    GtkListBox *admin_login_list;
    GtkListBox *deactivated_list;
    GtkWidget *name_label;
    GtkWidget *nickname_entry;
    GtkWidget *nickname_error_label;
    GtkWidget *nickname_success_label;
    GtkWidget *password_error_label;
    GtkWidget *password_success_label;
    GtkWidget *new_password_entry_1;
    GtkWidget *new_password_entry_2;
    GtkWidget *old_password_entry;
    GtkWidget *activate_entry;
    GtkWidget *deactivate_success_label;
    GtkWidget *create_error_label;  // For create user error messages
    GtkWidget *create_success_label;  // For create user success messages
    GtkWidget *login_entry;         // For login field
    GtkWidget *password_entry;      // For password field
    GtkWidget *admin_password_entry;
    GtkWidget *admin_error_label;
    GtkWidget *admin_success_label;
    GtkWidget *selected_user_label;
    GtkWidget *admin_login_entry;
    GtkWidget *delete_entry;      // Entry field for deactivation
    GtkWidget *error_label;       // General error label
    GtkWidget *success_label; 
    GtkWidget *deactivate_error_label;
    GtkWidget *activate_success_label; 
    GtkWidget *activate_error_label; 
    GtkWidget *right_side_container; 
} profile_data_t;

typedef struct {
    //GtkWidget *messages_container;
    char *username;
    int user_id; 
    char *session_id;
    bool is_admin; 
    call_data_t *call_data;
    chat_manager_t *chat_manager;
    chat_manager_t *group_manager;
    profile_data_t *profile_data;
    GtkWidget *window;
    GtkWidget *profile_window;
    GtkWidget *search_bar;
    pthread_mutex_t pthread_mutex;
    pthread_cond_t pthread_cond;
    bool login_successful;
    pthread_t recv_thread;
    bool *stop_login;

    GtkWidget *message_entry;
    GtkWidget *entry_overlay;

    pthread_t reconnect_thread;
    bool *stop_reconnect;
} GTK_data_t;

//extern GtkWidget *messages_container;
void apply_css(char* path);
void add_message(const char *message_text, const char *time_text, gboolean is_sent, bool changed, chat_manager_t *manager, SSL* ssl, int msg_id, chat_data_t *chat, char *nicknake);
void add_message_to_top(const char *message_text, const char *time_text, gboolean is_sent, bool changed, chat_manager_t *manager, SSL* ssl, int msg_id, chat_data_t *chat, char *nicknake);
void sleep_ms(int milliseconds);
void scroll_to_bottom(GtkWidget *container);
void on_send_clicked (GtkWidget *widget, gpointer user_data);
GtkWidget* create_message_container(chat_data_t *chat_data);
GtkWidget* create_chat_item(const char *name, int chat_id, const char *message, const char *time, gboolean is_online, gboolean is_group, GTK_data_t *GTK_data);
chat_data_t* create_chat_data(const char *contact_name, int contact_id, scroll_data_t *scroll_data);
void switch_chat(GtkWidget *widget, GTK_data_t *GTK_data);
void change_sidebar_chat_info(chat_data_t *chat, char *message, char *time);
void change_status_sidebar_chat(chat_data_t *chat, bool is_online);
void on_scroll_changed(GtkAdjustment *adjustment, gpointer user_data);
message_data_t* create_message_data (int message_id, chat_data_t* chat);
void on_message_edit(GtkGestureClick *gesture);
void change_message_from_others(chat_data_t *chat, int msg_id, char* new_message);
void delete_message_from_others(chat_data_t *chat, int msg_id);

void GTK_start(GTK_data_t *GTK_data);
void input_saved_data(GtkWidget *login_entry, GtkWidget *password_entry, GtkWidget *remember_me_check);
void check_remember_me(GtkWidget *remember_me_check, const char *login, const char *password);
void start_login(call_data_t *call_data);
void profile_start(GTK_data_t *GTK_data);
void profile_css(GtkWidget *widget);
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
void display_ui_message(GTK_data_t *GTK_data, const char *message, bool is_success);
void on_activate_button_clicked(GtkButton *button, gpointer user_data);
void on_deactivate_button_clicked(GtkButton *button, gpointer user_data);
gboolean reenable_button_after_delay(gpointer user_data);
void on_admin_change_password_button_clicked(GtkButton *button, gpointer user_data);
char *wrap_text_to_width(const char *message_text, int max_width, PangoContext *context);
void prepare_message_label(GtkWidget *message_label, const char *message_text);
