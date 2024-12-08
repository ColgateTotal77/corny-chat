//
// Created by konstantin on 03.11.24.
//

#ifndef SQL_H
#define SQL_H
#include <stdbool.h>
#include <sqlite3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct User_create {
    char login[40];
    char nickname[50];
    unsigned char password[32];
    int role_id;
} user_create;

typedef struct User {
    int id;
    char createdAt[20];
    char login[40];
    char nickname[50];
    int role_id;
    bool active;
} s_user;

typedef struct message {
    int id;
    char createdAt[20];
    char updatedAt[20];
    int owner_id;
    int target_id;
    int target_group_id;
    char message[512];
    bool readed;
} s_message;

typedef struct texting {
    int user1_id;
    int user2_id;
    int unread_mes_qty;
    int all_mes_qty;
    s_message *messages;
} s_texting;


typedef struct unread_messages {
    int sender_id;
    int unread_mes_qty;
} s_unread;

typedef struct Chat_user {
    int id;
    char nickname[50];
    bool active;
    int unread_mes_qty; // -1 if NO messaging
} chat_user;

typedef struct group {
    int id;
    char name[50];
    char createdAt[20];
    int owner_id;
    int occupants_num;
    int *occupants; //int array of the group occupants
    int unread_mes_qty;
} s_group;

int select_user_by_id(sqlite3 *db, s_user *user, int id);

void init_user(s_user *usr, const int id, const char *created_at, const char *login,
               const char *nickname,
               const int role_id, const bool active);

int get_usr_qty(sqlite3 *db);

s_user *select_all_users(sqlite3 *db);

int create_user(sqlite3 *db, user_create usr);

void init_user_create(user_create *usr, const char *login, const char *nickname,
                      const unsigned char *pas_hash, const int role_id);

unsigned char *get_password_hash(sqlite3 *db, const char *login);

int update_password_hash(sqlite3 *db, const int usr_id, const unsigned char *hash);

int update_nickname(sqlite3 *db, const int usr_id, const char *new_nickname);

int insert_private_message(sqlite3 *db, int owner_id, int target_id,
                           char *message, unsigned char *s_key);

void init_message(s_message *msg, const int id, const char *created_at, const char *updated_at,
                  const int owner_id, const int target_id, const int target_group_id,
                  char *message,const bool readed);

int get_message_by_id(sqlite3 *db, s_message *message, const int mes_id);

s_message *get_new_mess_between(sqlite3 *db, const int user1_id, 
                                const int user2_id, int *mes_qty);

s_unread *get_senders_list(sqlite3 *db, int receiver_id, int *senders_num);

s_texting *get_starting_messages(sqlite3 *db, const int user_id, int *senders_qty);

void free_texting(s_texting *textings, const int senders_qty);

int set_mes_read_status(sqlite3 *db, int user_id, int sender_id);

s_texting *get_last_messages_between(sqlite3 *db, const int usr1_id, const int usr2_id,
                                     const int qty, const int before);

void init_chat_user(chat_user *usr, const int id, const char *nickname, const bool active,
                    const int unread_mes_qty);

chat_user *get_clients_userslist(sqlite3 *db, const int usr_id, bool show_unknown, int *usr_qty);

bool get_usr_status(sqlite3 *db, const char *login);

int add_users_to_group(sqlite3 *db, int group_id, const int *users_list, const int list_len);

int create_group(sqlite3 *db, const int owner_id, char *name, const int *users_list,
                 const int list_len);

int delete_usr_from_group(sqlite3 *db, const int user_id, const int group_id);

int delete_group(sqlite3 *db, const int group_id);

s_user *get_group_users(sqlite3 *db, int group_id, int *usr_qty);

int *get_group_occupants_list(sqlite3 *db, int group_id, const int occupants_num);

void init_s_group(s_group *group, const int id, char *name, char *created_at,
                  const int owner_id, int occupants_num, int *occupants);

s_group *get_groups_full_list(sqlite3 *db, const int user_id, int* group_qty);

void free_groups_full_list(s_group* groups, const int groups_qty);

int activate_deactivate_user(sqlite3 *db, const int user_id, const bool is_active);

int insert_group_message(sqlite3 *db, int owner_id, int target_group_id, char *message,
                         unsigned char *s_key);

s_message *get_new_group_mess(sqlite3 *db, int group_id, int required_qty,
                              int *act_arr_len, int before);

int update_message(sqlite3 *db, int message_id, char *new_message);

int delete_message(sqlite3 *db, int message_id);

int set_group_mes_read_status(sqlite3 *db, int user_id, int group_id);



#endif //SQL_H
