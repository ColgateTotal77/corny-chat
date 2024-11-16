//
// Created by konst on 07.11.24.
//
#include "../../inc/sql.h"


/**
 * @brief updates user nickname
 *
 * @param db
 * @param usr_id
 * @param new_nickname
 * @return 0 if Ok else sql error code
 */
int update_nickname(sqlite3* db, const int usr_id, const char* new_nickname) {
	const char * sql = "UPDATE users SET nickname = ? WHERE id = ?;";
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return rc;
	}

	sqlite3_bind_text(stmt, 1, new_nickname, strlen(new_nickname), SQLITE_STATIC);
	sqlite3_bind_int(stmt, 2, usr_id);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return rc;
	}
	sqlite3_finalize(stmt);
	printf("Users '%s' nickname updated successfully.\n", new_nickname);
	return rc;

}

