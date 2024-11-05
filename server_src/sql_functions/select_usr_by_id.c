//
// Created by konstantin on 04.11.24.
//

#include "../../inc/sql.h"

/**
 *
 * @brief select a user from DB by his id and adds to appropriate structure
 * @param db opened Data Base connection
 * @param user s_user structure from sql.h
 * @param id user ID
 * @return success or error code
 */
int select_user_by_id(sqlite3 *db, s_user *user, int id) {
    const char *sql = "SELECT id, createdAt, login, nickname, roleId, active FROM users WHERE id = ?";
    sqlite3_stmt *stmt;

    // Подготовка SQL-запроса
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Установка параметра
    sqlite3_bind_int(stmt, 1, id);
    // Выполнение запроса и получение результатов
    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        init_user(user,
                  sqlite3_column_int(stmt, 0),
                  (const char *) sqlite3_column_text(stmt, 1),
                  (const char *) sqlite3_column_text(stmt, 2),
                  (const char *) sqlite3_column_text(stmt, 3),
                  sqlite3_column_int(stmt, 4),
                  (bool)sqlite3_column_int(stmt, 5));
    } else {
        fprintf(stderr, "User not found: %s\n", sqlite3_errmsg(db));
    }

    // Освобождение ресурсов
    sqlite3_finalize(stmt);
    return rc == SQLITE_ROW ? SQLITE_OK : rc;
}
