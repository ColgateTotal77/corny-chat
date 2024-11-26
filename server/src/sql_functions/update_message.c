//
// Created by konst on 25.11.24.
//

#include "../../inc/sql.h"

/**
 * @brief updates message in DB
 * @warning updating must only be permitted for the author
 *
 * @param db
 * @param message_id
 * @param new_message
 * @return DB response code (101 if OK)
 */
int update_message(sqlite3 *db, int message_id, char *new_message) {
    char* sql = "UPDATE messages"
                " SET updatedAt = CURRENT_TIMESTAMP,"
                " message = ?"
                " WHERE id = ? ";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, new_message, strlen(new_message), SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, message_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    sqlite3_finalize(stmt);
    printf("Message '%s' was updated successfully.\n", new_message);
    return rc;
}
