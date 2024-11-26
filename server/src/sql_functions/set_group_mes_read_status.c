//
// Created by konst on 26.11.24.
//

#include "../../inc/sql.h"

/** set group-member-unread-messages quantity to 0
 *
 * @param db
 * @param user_id
 * @param group_id
 * @return DB response code (101 if OK)
 */
int set_group_mes_read_status(sqlite3 *db, int user_id, int group_id) {
    char* sql = "UPDATE group_users SET unreadNum = 0 WHERE groupId = ? AND userId = ?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, group_id);
    sqlite3_bind_int(stmt, 2, user_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    sqlite3_finalize(stmt);
    return rc;
}