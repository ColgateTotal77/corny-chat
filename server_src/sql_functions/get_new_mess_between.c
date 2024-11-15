//
// Created by konst on 10.11.24.
//

#include "../../inc/sql.h"

/**
 *
 * @brief returns all messages after EARLIEST UNREAD MESSAGE and 5(currently) messages before.
 * function allocates memory DO NOT FORGET FREE
 *
 * @param db
 * @param user1_id
 * @param user2_id
 * @param mes_qty takes pointer to int variable and save array len
 * @return pointer to array  of sorted messages
 */
s_message* get_new_mess_between(sqlite3 *db, const int user1_id, const int user2_id, int *mes_qty){
	const int num_of_readed = 5;
	const char* sql = "SELECT m1.id, m1.createdAt, m1.ownerId, m1.targetUserId, m1.targetGroupId, m1.message, m1.readed"
						" FROM messages m1"
						" WHERE ((m1.ownerId = ? AND m1.targetUserId = ? ) OR (m1.ownerId = ? AND m1.targetUserId = ? )) and m1.createdAt >= ("
							" SELECT MIN(m.createdAt) as earlyest_unread"
							" FROM messages m"
							" WHERE m.readed = FALSE AND ((m.ownerId = ? AND m.targetUserId = ? ) OR (m.ownerId = ? AND m.targetUserId = ? )))"
						" UNION"
						" SELECT * FROM "
							" (SELECT m1.id, m1.createdAt, m1.ownerId, m1.targetUserId, m1.targetGroupId, m1.message, m1.readed"
							" FROM messages m1 "
							" WHERE ((m1.ownerId = ? AND m1.targetUserId = ? ) OR (m1.ownerId = ? AND m1.targetUserId = ? )) and m1.createdAt < ("
								" SELECT MIN(m.createdAt) as earlyest_unread "
								" FROM messages m"
								" WHERE m.readed = FALSE AND ((m.ownerId = ? AND m.targetUserId = ? ) OR (m.ownerId = ? AND m.targetUserId = ? )))"
							" LIMIT ?)"
						" ORDER BY readed DESC, createdAt ;";
	sqlite3_stmt* stmt;

	// Подготовка SQL-запроса
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return NULL;
	}

	// Установка параметра
	sqlite3_bind_int(stmt, 1, user2_id);
	sqlite3_bind_int(stmt, 2, user1_id);
	sqlite3_bind_int(stmt, 3, user1_id);
	sqlite3_bind_int(stmt, 4, user2_id);
	sqlite3_bind_int(stmt, 5, user2_id);
	sqlite3_bind_int(stmt, 6, user1_id);
	sqlite3_bind_int(stmt, 7, user1_id);
	sqlite3_bind_int(stmt, 8, user2_id);
	sqlite3_bind_int(stmt, 9, user2_id);
	sqlite3_bind_int(stmt, 10, user1_id);
	sqlite3_bind_int(stmt, 11, user1_id);
	sqlite3_bind_int(stmt, 12, user2_id);
	sqlite3_bind_int(stmt, 13, user2_id);
	sqlite3_bind_int(stmt, 14, user1_id);
	sqlite3_bind_int(stmt, 15, user1_id);
	sqlite3_bind_int(stmt, 16, user2_id);
	sqlite3_bind_int(stmt, 17, num_of_readed);

	int count = 0;
	while ((sqlite3_step(stmt)) == SQLITE_ROW) {
		count++;
	}
	*mes_qty = count;
	s_message *messages = malloc(count * sizeof(s_message));
	if (!messages) {
		fprintf(stderr, "Memory allocation failed\n");
		sqlite3_finalize(stmt);
		return NULL;
	}
	// printf("user 1 id = %d\n ", user1_id);
	// printf("user 2 id = %d\n ", user2_id);
	// printf("size off messages array = %d\n\n", *mes_qty);

	// Сбрасываем состояние запроса для повторного использования
	sqlite3_reset(stmt);
	int i = 0;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		init_message(&messages[i],
					 sqlite3_column_int(stmt, 0),
					 (const char*)sqlite3_column_text(stmt, 1),
					 sqlite3_column_int(stmt, 2),
					 sqlite3_column_int(stmt, 3),
					 sqlite3_column_int(stmt, 4),
					 (char*)sqlite3_column_text(stmt, 5),
					 (bool)sqlite3_column_int(stmt, 6));
		i++;
	}

	sqlite3_finalize(stmt);
	return messages;
}
