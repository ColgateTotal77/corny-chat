//
// Created by konstantin on 03.11.24.
//

#ifndef SQL_H
#define SQL_H
#include <stdbool.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>





typedef struct User_create {
    char login[50];
    char nickname[50];
    char password[50];
    int role_id;
} user_create;

typedef struct User {
    int id;
    char createdAt[20];
    char login[50];
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


#endif //SQL_H
