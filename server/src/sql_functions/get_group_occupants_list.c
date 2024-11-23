//
// Created by konst on 22.11.24.
//

#include "../../inc/sql.h"

/**
 * @brief returns array of user id that are in the group
 * @warning function allocates memory. Do not forget free
 *
 * @param db
 * @param group_id
 * @param occupants_num you have to know number of occupants in the group
 * @return pointer to array[int] or NULL if something went wrong
 */
int *get_group_occupants_list(sqlite3 *db, int group_id, const int occupants_num) {
    const char *sql = "SELECT userId "
            "FROM group_users "
            "WHERE groupId = ?;";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    sqlite3_bind_int(stmt, 1, group_id);
    int *list = malloc(sizeof(int) * occupants_num);
    if (list == NULL) {
        printf("Failed to allocate memory for list\n");
        sqlite3_finalize(stmt);
        return NULL;
    }
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list[i] = sqlite3_column_int(stmt, 0);
        i++;
    }
    sqlite3_finalize(stmt);
    return list;
}
