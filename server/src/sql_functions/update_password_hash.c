//
// Created by konst on 07.11.24.
//

#include <sqlite3.h>
#include <stddef.h>
#include <stdio.h>

/**
 *@brief updates users password hash in DB
 *
 * @param db
 * @param usr_id
 * @param hash
 * @return
 */
int update_password_hash(sqlite3* db, const int usr_id, const unsigned char* hash) {
	const char* sql = "UPDATE users SET password = ? WHERE id = ?;";
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return rc;
	}

	sqlite3_bind_blob(stmt, 1, hash, 32, SQLITE_STATIC); // Привязываем хеш пароля
	sqlite3_bind_int(stmt, 2, usr_id);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return rc;
	}
	sqlite3_finalize(stmt);
	printf("Users '%d' password updated successfully.\n", usr_id);
		return rc;
}
