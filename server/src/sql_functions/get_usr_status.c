//
// Created by konst on 19.11.24.
//

#include "../../inc/sql.h"
#include <stdbool.h>
#include <stdio.h>

/**
 *
 * @param db
 * @param login
 * @return True if user is active else False
 */
bool get_usr_status(sqlite3* db, const char *login) {
	const char *sql = "SELECT active FROM users WHERE login LIKE ?";
	sqlite3_stmt *stmt;

	// Подготовка SQL-запроса
	const int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return NULL;
	}

	// Установка параметра
	sqlite3_bind_text(stmt, 1, login, -1, SQLITE_STATIC);

	bool status = false;
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		status = sqlite3_column_int(stmt, 0);
	} else {
		fprintf(stderr, "User not found: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(stmt);
	return  status;
}
