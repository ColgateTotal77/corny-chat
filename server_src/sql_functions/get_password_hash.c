//
// Created by konstantin on 06.11.24.
//

#include "../../inc/sql.h"

/**
 * @brief returns password cash from DB by login
 * WARNING function allocate memory do not forget free
 *
 * @param db opened Data Base connection
 * @param login user login
 * @return pointer to allocated unsigned char with hash 32 B len
 * @return NULL if something went wrong (look at error prints)
 */
unsigned char *get_password_hash(sqlite3 *db, const char *login) {
    const char *sql = "SELECT password FROM users WHERE login LIKE ?";
    sqlite3_stmt *stmt;

    // Подготовка SQL-запроса
    const int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    // Установка параметра
    sqlite3_bind_text(stmt, 1, login, -1, SQLITE_STATIC);
    // Выполнение запроса и получение результатов

    unsigned char *hash = malloc(sizeof(unsigned char) *32);
    if (hash == NULL) {
        fprintf(stderr, "Failed to allocate memory for hash\n");
        return NULL;
    }
    memset(hash, 0, 32);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *password = sqlite3_column_blob(stmt, 0);
        if (password == NULL) {
            fprintf(stderr, "password hash is NULL user: %s\n", login);
            free(hash);
            return NULL;
        }
        memcpy(hash, password, 32);
    } else {
        fprintf(stderr, "User not found: %s\n", sqlite3_errmsg(db));
        free(hash);
        sqlite3_finalize(stmt);
        return NULL;
    }

    sqlite3_finalize(stmt);
    return  hash;
}

