//
// Created by konst on 09.11.24.
//

#include "../../inc/sql.h"

/**
 * @brief select a message from DB by its id and adds to appropriate structure
 *
 * @param db
 * @param message
 * @param mes_id
 * @return SQLITE_OK or rc code
 */
int get_message_by_id(sqlite3* db, s_message *message, const int mes_id) {
	const char* sql = "SELECT m.id, m.createdAt, m.ownerId, m.targetUserId, m.targetGroupId, m.message, m.readed "
		"FROM messages m "
		"WHERE id = ?;";
	sqlite3_stmt* stmt;

	// Подготовка SQL-запроса
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	// Установка параметра
	sqlite3_bind_int(stmt, 1, mes_id);
	// Выполнение запроса и получение результатов
	if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		init_message(message,
		             sqlite3_column_int(stmt, 0),
		             (const char*)sqlite3_column_text(stmt, 1),
		             sqlite3_column_int(stmt, 2),
		             sqlite3_column_int(stmt, 3),
		             sqlite3_column_int(stmt, 4),
		             (char*)sqlite3_column_text(stmt, 5),
		             (bool)sqlite3_column_int(stmt, 6));
	}else {
		fprintf(stderr, "message not found: %s\n", sqlite3_errmsg(db));
	}
	sqlite3_finalize(stmt);
	return rc == SQLITE_ROW ? SQLITE_OK : rc;
}
