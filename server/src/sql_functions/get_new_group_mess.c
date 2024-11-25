//
// Created by konst on 25.11.24.
//

#include "../../libmx/inc/libmx.h"

#include "../../inc/sql.h"

/**
 * @brief returns group message story
 * @warning function allocates memory. Use free
 *
 * @param db
 * @param group_id
 * @param required_qty number of messages that tou want to receive
 * @param act_arr_len  save number of messages that was received actually
 * @param before must be <0> or the earliest sent message_ID
 * @return array[s_message]
 */
s_message *get_new_group_mess(sqlite3 *db, int group_id, int required_qty,
                              int *act_arr_len, int before) {
	sqlite3_stmt *stmt;
	int last_mess_id = before ? before : INT_MAX;

	const char* sql = "SELECT * "
					"FROM "
						"("
						"SELECT "
							"m.id, "
							"m.createdAt, "
							"m.updatedAt, "
							"m.ownerId, "
							"m.targetUserId,"
							"m.targetGroupId, "
							"m.message, "
							"m.readed "
						"FROM "
							"messages m "
						"WHERE "
							"m.targetGroupId = ? "
							"AND m.id < ?"
						"ORDER BY "
							"m.id DESC "
						"LIMIT ?) "
					"ORDER BY "
						"id";

	// printf("%s\n", sql);


	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return NULL;
	}

	sqlite3_bind_int(stmt, 1, group_id);
	sqlite3_bind_int(stmt, 2, last_mess_id);
	sqlite3_bind_int(stmt, 3, required_qty);

	t_list* list = NULL;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		s_message *message = malloc(sizeof(s_message));
		init_message(message,
		             sqlite3_column_int(stmt, 0),
		             (const char*)sqlite3_column_text(stmt, 1),
		             (const char*)sqlite3_column_text(stmt, 2),
		             sqlite3_column_int(stmt, 3),
		             sqlite3_column_int(stmt, 4),
		             sqlite3_column_int(stmt, 5),
		             (char*)sqlite3_column_text(stmt, 6),
		             (bool)sqlite3_column_int(stmt, 7));
		mx_push_back(&list, message);
	}
	*act_arr_len = mx_list_size(list);
	s_message *messages = malloc(*act_arr_len * sizeof(s_message));
	if (!messages) {
		fprintf(stderr, "Memory allocation failed\n");
		mx_t_list_deep_free(list);
		sqlite3_finalize(stmt);
		return NULL;
	}
	if (*act_arr_len > 0) {
		t_list *temp_list = list;
		int i = 0;
		while (temp_list) {
			const s_message *message = temp_list->data;
			messages[i] = *message;
			temp_list = temp_list->next;
			i++;
		}
	}
	mx_t_list_deep_free(list);
	sqlite3_finalize(stmt);
	return messages;
}
