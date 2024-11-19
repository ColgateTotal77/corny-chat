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
    char login[15];
    char nickname[50];
    unsigned char password[32];
    int role_id;
} user_create;

typedef struct User {
    int id;
    char createdAt[20];
    char login[15];
    char nickname[50];
    int role_id;
    bool active;
} s_user;

typedef struct message {
	int id;
	char createdAt[20];
	int owner_id;
	int target_id;
	int target_group_id;
	char message[512];
	bool readed;
} s_message;

typedef struct texting {
	int user1_id;
	int user2_id;
	int	unread_mes_qty;
	int all_mes_qty;
	s_message* messages;
} s_texting;

typedef struct unread_messages {
	int sender_id;
	int unread_mes_qty;
} s_unread;

typedef struct Chat_user {
	int id;
	char nickname[50];
	bool active;
	int unread_mes_qty;  // -1 if NO messaging
} chat_user;


int create_user(sqlite3 *db, user_create usr);

int sql_insert_msg(sqlite3 *db, int usr_id, char *msg);

int select_user_by_id(sqlite3 *db, s_user *user, int id);

void init_user(s_user *usr, const int id, const char *created_at, const char *login,
               const char *nickname,
               const int role_id, const bool active);

int get_usr_qty(sqlite3 *db);

s_user *select_all_users(sqlite3 *db);

void init_user_create(user_create *usr, const char *login, const char *nickname,
                      const unsigned char *pas_hash, const int role_id);
unsigned char *get_password_hash(sqlite3 *db, const char *login);

int update_password_hash(sqlite3* db, const int usr_id, const unsigned char* hash);
int update_nickname(sqlite3* db, const int usr_id, const char* new_nickname);
int insert_private_message(sqlite3* db, int owner_id, int target_id, char* message, unsigned char* s_key);
void init_message(s_message *msg, const int id, const char *created_at,
			   const int owner_id, const int target_id, const int target_group_id,
			   char message[512],const bool readed);
int get_message_by_id(sqlite3* db, s_message *message, const int mes_id);
s_message* get_new_mess_between(sqlite3 *db, const int user1_id, const int user2_id, int *mes_qty);
s_unread* get_senders_list(sqlite3* db, int receiver_id, int* senders_num);
s_texting* get_starting_messages(sqlite3* db, const int user_id, int* senders_qty);
void free_texting(s_texting* textings, const int senders_qty);
int set_mes_read_status(sqlite3* db, int user_id, int sender_id);
s_texting* get_last_messages_between(sqlite3* db, const int usr1_id, const int usr2_id,
									 const int qty, const int before);
void init_chat_user(chat_user* usr, const int id, const char* nickname, const bool active,
					const int unread_mes_qty);

chat_user* get_clients_userslist(sqlite3* db, const int usr_id, bool show_unknown, int* usr_qty) ;
bool get_usr_status(sqlite3* db, const char *login);

#endif //SQL_H
