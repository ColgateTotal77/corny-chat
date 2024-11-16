//
// Created by konst on 09.11.24.
//

#include "../../inc/sql.h"



/**
 * @brief adds private message to DB
 *
 * @param db
 * @param owner_id
 * @param target_id
 * @param message
 * @param s_key secret key can be NULL (not implemented yet )
 * @return message_id - id of saved message or -1 if something went wrong
 */
int insert_private_message(sqlite3* db, int owner_id, int target_id, char* message, unsigned char* s_key) {
	const char* sql = "INSERT INTO messages (ownerId, targetUserId,message,sKey) VALUES (?, ?, ?, ?);";
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	// Привязка параметров
	sqlite3_bind_int(stmt, 1, owner_id);
	sqlite3_bind_int(stmt, 2, target_id);
	sqlite3_bind_text(stmt, 3, message, -1, SQLITE_STATIC);
	sqlite3_bind_blob(stmt, 4, s_key, -1, SQLITE_STATIC);

	// Выполнение подготовленного выражения
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	// Получаем последний сгенерированный ID
	const int message_id = (int)sqlite3_last_insert_rowid(db);

	sqlite3_finalize(stmt);
	printf("message '%s' saved successfully.\n", message);
	return message_id;
}
