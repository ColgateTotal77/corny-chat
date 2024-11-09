//
// Created by konstantin on 04.11.24.
//
//#include <stdio.h>
//#include <sqlite3.h>
#include <stdlib.h>
#include "../../inc/sql.h"

/**
 *@brief adds new user to DB from appropriate structure
 *
 * @param db opened Data Base connection
 * @param usr user_create structure from sql.h
 *
 * @return created UserID or -1 if something went wrong
 */
int create_user(sqlite3 *db, user_create usr) {
    const char *sql = "INSERT INTO users (login, password, nickname, roleId) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Привязка параметров
    sqlite3_bind_text(stmt, 1, usr.login, -1, SQLITE_STATIC); // Привязываем логин
    sqlite3_bind_blob(stmt, 2, usr.password, sizeof(usr.password), SQLITE_STATIC); // Привязываем хеш пароля
    sqlite3_bind_text(stmt, 3, usr.nickname, -1, SQLITE_STATIC); // Привязываем никнейм
    sqlite3_bind_int(stmt, 4, usr.role_id); // Привязываем роль

    // Выполнение подготовленного выражения
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    // Получаем последний сгенерированный ID
    const int user_id = (int) sqlite3_last_insert_rowid(db);

    sqlite3_finalize(stmt);
    printf("User '%s' created successfully.\n", usr.login);
    return user_id;
}

