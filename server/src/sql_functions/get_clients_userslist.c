//
// Created by konst on 14.11.24.
//

#include "../../libmx/inc/libmx.h"
#include "../../inc/sql.h"

/**
 * @brief returns sorted array of users to perform them in the client chat.
 * Might return NULL in cases:
 *	1 something went wrong;
 *	2 there is no users in DB except usr_id
 *	3 show_unknown == FALSE and user have not had any messaging yet
 * for each usr_id, items in this list will be different
 * @warning this function allocate memory do not forget free
 *
 * @param db
 * @param usr_id
 * @param show_unknown if TRUE there'll be all users, else - users having messaging
 * @param usr_qty will save array size
 * @return NULL or array of chat_user structures
 */
chat_user* get_clients_userslist(sqlite3* db, const int usr_id, bool show_unknown, int* usr_qty) {
	char* sql = NULL;
	if (show_unknown) {
		sql = "SELECT u.id, u.nickname, u.active, IFNULL (mes.unread, -1) AS unread "
			"FROM users u LEFT JOIN (SELECT COALESCE(NULLIF(m.targetUserId, ?), "
			"m.ownerId) AS user_id, "
			"SUM (CASE WHEN m.targetUserId = ? AND m.readed IS FALSE THEN 1 ELSE 0 END) AS unread "
			"FROM messages m "
			"WHERE "
			"m.targetUserId = ? "
			"OR m.ownerId = ? "
			"GROUP BY "
			"user_id) mes  ON u.id = mes.user_id "
			"WHERE u.id !=?"
			"ORDER BY mes.unread DESC, u.nickname ";
	}
	else {
		sql = "SELECT u.id, u.nickname, u.active, IFNULL (mes.unread, -1) AS unread "
			"FROM users u JOIN (SELECT COALESCE(NULLIF(m.targetUserId, ?), "
			"m.ownerId) AS user_id, "
			"SUM (CASE WHEN m.targetUserId = ? AND m.readed IS FALSE THEN 1 ELSE 0 END) AS unread "
			"FROM messages m "
			"WHERE "
			"m.targetUserId = ? "
			"OR m.ownerId = ? "
			"GROUP BY "
			"user_id) mes  ON u.id = mes.user_id "
			"ORDER BY mes.unread DESC, u.nickname ";
	}
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK) {
		printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return NULL;
	}

	sqlite3_bind_int(stmt, 1, usr_id);
	sqlite3_bind_int(stmt, 2, usr_id);
	sqlite3_bind_int(stmt, 3, usr_id);
	sqlite3_bind_int(stmt, 4, usr_id);
	if (show_unknown) sqlite3_bind_int(stmt, 5, usr_id);

	t_list* list = NULL;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		chat_user* user = malloc(sizeof(chat_user));
		init_chat_user(user,
		               sqlite3_column_int(stmt, 0),
		               (const char*)sqlite3_column_text(stmt, 1),
		               sqlite3_column_int(stmt, 2),
		               sqlite3_column_int(stmt, 3));
		mx_push_back(&list, user);
	}
	*usr_qty = mx_list_size(list);
	chat_user* users = malloc(sizeof(chat_user) * (*usr_qty));
	if (!users) {
		mx_t_list_deep_free(list);
		return NULL;
	}
	if (*usr_qty > 0) {
		t_list* temp_list = list;
		int i = 0;
		while (temp_list) {
			chat_user* user = temp_list->data;
			users[i] = *user;
			temp_list = temp_list->next;
			i++;
		}
	}
	mx_t_list_deep_free(list);
	sqlite3_finalize(stmt);
	return users;
}
