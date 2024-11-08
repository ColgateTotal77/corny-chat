//
// Created by konstantin on 03.11.24.
//

#ifndef SQL_H
#define SQL_H
#include <stdbool.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include  <string.h>


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

int insert_user(sqlite3 *db, user_create usr);

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

#endif //SQL_H
