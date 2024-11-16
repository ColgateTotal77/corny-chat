//
// Created by konstantin on 05.11.24.
//
#include "../../inc/sql.h"

/**
 * @param db active DB connection
 * @return number of users or -1 if something went wrong
 */
int get_usr_qty(sqlite3 *db) {
    const char *sql = "SELECT COUNT(*) FROM users;";
    sqlite3_stmt *stmt;
    int qty = 0;

    // Подготовка SQL-запроса
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1; // Ошибка подготовки запроса
    }

    // Выполнение запроса
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Извлечение количества пользователей
        qty = sqlite3_column_int(stmt, 0);
    } else {
        fprintf(stderr, "Failed to retrieve user count: %s\n", sqlite3_errmsg(db));
        qty = -1; // Ошибка выполнения запроса
    }

    // Освобождение ресурсов
    sqlite3_finalize(stmt);
    return qty;
}
