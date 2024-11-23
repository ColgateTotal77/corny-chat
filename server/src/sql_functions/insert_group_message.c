//
// Created by konst on 23.11.24.
//

#include "../../inc/sql.h"


/**
 * @brief adds private message to DB
 *
 * @param db
 * @param owner_id
 * @param target_group_id
 * @param message
 * @param s_key secret key can be NULL (not implemented yet )
 * @return message_id - id of saved message or -1 if something went wrong
 */
int insert_group_message(sqlite3 *db, int owner_id, int target_group_id, char *message,
                         unsigned char *s_key) {
    sqlite3_stmt *stmt, *stmt2;
    int rc;

    const char *sql = "INSERT INTO messages (ownerId, targetGroupId,message,sKey) VALUES (?, ?, ?, ?);";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement1: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Привязка параметров
    sqlite3_bind_int(stmt, 1, owner_id);
    sqlite3_bind_int(stmt, 2, target_group_id);
    sqlite3_bind_text(stmt, 3, message, -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 4, s_key, -1, SQLITE_STATIC);

    // Выполнение подготовленного выражения
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    // Получаем последний сгенерированный ID
    const int message_id = (int) sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);

    const char* sql2 = "UPDATE group_users SET unreadNum = unreadNum +1 WHERE groupId = ?;";
    rc = sqlite3_prepare_v2(db, sql2, -1, &stmt2, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement2: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt2, 1, target_group_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);

    printf("message '%s' saved successfully.\n", message);
    return message_id;
}
