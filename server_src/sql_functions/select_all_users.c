//
// Created by konstantin on 05.11.24.
//

#include "../../inc/sql.h"


/**
 *
 * @brief gets all users from DB and adds to appropriate structures
 * WARRING uses allocated memory, use <free>
 *
 * @param db active DB connection
 * @return array of all users (allocated)
 */
s_user *select_all_users(sqlite3 *db) {
    const int user_qty = get_usr_qty(db);
    s_user *users = malloc(sizeof(s_user) * user_qty);
    if (users == NULL) {
        return NULL;
    }
    const char *sql = "SELECT id, createdAt, login, nickname, roleId, active FROM users;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL; // Ошибка подготовки запроса
    }
    int i = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        init_user(&users[i],
                  sqlite3_column_int(stmt, 0),
                  (const char *) sqlite3_column_text(stmt, 1),
                  (const char *) sqlite3_column_text(stmt, 2),
                  (const char *) sqlite3_column_text(stmt, 3),
                  sqlite3_column_int(stmt, 4),
                  (bool) sqlite3_column_int(stmt, 5));
        i++;
    }
    sqlite3_finalize(stmt);
    return users;
}
