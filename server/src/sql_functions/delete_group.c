//
// Created by konst on 21.11.24.
//

#include "../../inc/sql.h"

/**
 * @warning this func must be allowed only for group owner
 *
 * @param db
 * @param group_id
 * @return -1 if something went wrong
 * @return 0 if group doesn't exist
 * @return 1 if Ok
 */
int delete_group(sqlite3* db, const int group_id) {
	const char* sql = "DELETE FROM groups WHERE id = ?;";
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	sqlite3_bind_int(stmt, 1, group_id);

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
