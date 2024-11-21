//
// Created by konst on 21.11.24.
//

#include "../../inc/sql.h"

/**
 * @warning this func must be allowed only for group owner
 *
 * @param db
 * @param user_id
 * @param group_id
 * @return 1 if Ok else -1
 * @return 0 if nothing has been deleted (row doesn't exist)
 */
int delete_usr_from_group(sqlite3* db, const int user_id, const int group_id) {
	const char* sql = "DELETE FROM group_users WHERE groupId = ? AND userId = ?;";
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	// Привязка параметров
	sqlite3_bind_int(stmt, 1, group_id);
	sqlite3_bind_int(stmt, 2, user_id);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	int deleted_num = (int)sqlite3_changes(db);
	sqlite3_finalize(stmt);
	return deleted_num;
}
