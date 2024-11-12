//
// Created by konst on 11.11.24.
//

#include "../../inc/sql.h"


/**
 *
 * @param db
 * @param user_id
 * @param sender_id
 * @return number of set up messages or -1 if something went wrong
 */
int set_mes_read_status(sqlite3* db, const int user_id, const int sender_id) {
	char* sql = "UPDATE messages SET readed = TRUE WHERE targetUserId = ? AND ownerId = ? AND readed = FALSE;";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK) {
		printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	sqlite3_bind_int(stmt, 1, user_id);
	sqlite3_bind_int(stmt, 2, sender_id);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	int updated_rows = sqlite3_changes(db);
	sqlite3_finalize(stmt);
	return updated_rows;
}
