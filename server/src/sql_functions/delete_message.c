//
// Created by konst on 26.11.24.
//

#include "../../inc/sql.h"

/**
 * @brief deletes message from DB
 * @warning deletion must only be permitted for the author
 *
 * @param db
 * @param message_id
 * @return 1 if OK, 0 if message_id do not exist, SQL response code if something went wrong
 */
int delete_message(sqlite3 *db, const int message_id) {
    char *sql = "DELETE FROM messages WHERE id = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Привязка параметров
    sqlite3_bind_int(stmt, 1, message_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    int deleted_num = (int) sqlite3_changes(db);
    sqlite3_finalize(stmt);
    return deleted_num;
}
