//
// Created by konst on 21.11.24.
//

#include "../../inc/sql.h"
#include "../../libmx/inc/libmx.h"


/**
 * @brief gets all the users of the group
 * @warning function allocates memory, do not forget free
 *
 * @param db
 * @param group_id
 * @param usr_qty saves number of users
 * @return pointer to array of s_user structures
 */
s_user* get_group_users(sqlite3* db, const int group_id, int* usr_qty) {
	const char* sql = "SELECT u.id, u.createdAt, u.login, u.nickname, u.roleId, u.active "
		"FROM users u JOIN group_users gu ON u.id = gu.userId "
		"WHERE gu.groupId = ?;";
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return NULL; // Ошибка подготовки запроса
	}

	sqlite3_bind_int(stmt, 1, group_id);

	t_list* list = NULL;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		s_user* user = malloc(sizeof(s_user));
		init_user(user,
		          sqlite3_column_int(stmt, 0),
		          (const char*)sqlite3_column_text(stmt, 1),
		          (const char*)sqlite3_column_text(stmt, 2),
		          (const char*)sqlite3_column_text(stmt, 3),
		          sqlite3_column_int(stmt, 4),
		          (bool)sqlite3_column_int(stmt, 5));
		mx_push_back(&list, user);
	}
	*usr_qty = mx_list_size(list);
	s_user* users = malloc(sizeof(s_user) * (*usr_qty));

	if (!users) {
		mx_t_list_deep_free(list);
		return NULL;
	}

	if (*usr_qty > 0) {
		t_list* temp_list = list;
		int i = 0;
		while (temp_list) {
			s_user* user = temp_list->data;
			users[i] = *user;
			temp_list = temp_list->next;
			i++;
		}
	}
	mx_t_list_deep_free(list);
	sqlite3_finalize(stmt);
	return users;
}
