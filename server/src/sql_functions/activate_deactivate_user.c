//
// Created by konst on 23.11.24.
//

#include "../../inc/sql.h"


/**
 * @brief save deletion/restoring of the user
 * @warning only admin is permitted to use
 *
 * @param db
 * @param user_id
 * @param is_active sets users status to this condition
 * @return response code (101 if OK)
 */
int activate_deactivate_user(sqlite3 *db, const int user_id, const bool is_active) {
    char *sql = "UPDATE users SET active = ? WHERE id = ? ;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, is_active);
    sqlite3_bind_int(stmt, 2, user_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    sqlite3_finalize(stmt);
    printf("Users '%d' active status is updated successfully = '%hhd'.\n", user_id, is_active);
    return rc;
}

