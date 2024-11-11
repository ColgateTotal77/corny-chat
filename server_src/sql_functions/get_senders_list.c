//
// Created by konst on 11.11.24.
//

#include "../../inc/sql.h"
#include "../../libmx_konst/inc/libmx.h"

void print_unread_message(const s_unread* message);
void print_list(t_list* list);

/**
 * @brief returns array of structures that contain number of unread messages and sender id
 *
 * @param db
 * @param receiver_id id of user who has to receive messages
 * @param senders_num pointer to int var that will save array_len
 * @return array of structures
 */
s_unread* get_senders_list(sqlite3* db, int receiver_id, int* senders_num) {
	char* sql = "SELECT m.ownerId, COUNT(*) num FROM messages m "
		"WHERE m.targetUserId = ? AND m.readed = FALSE GROUP BY m.ownerId;";

	sqlite3_stmt* stmt;
	// Подготовка SQL-запроса
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return NULL;
	}
	t_list* list = NULL;
	// Установка параметра
	sqlite3_bind_int(stmt, 1, receiver_id);
	// Выполнение запроса и получение результатов
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		s_unread* unread = malloc(sizeof(s_unread));
		unread->sender_id = sqlite3_column_int(stmt, 0);
		unread->unread_mes_qty = sqlite3_column_int(stmt, 1);
		mx_push_back(&list, unread);
	}
	*senders_num = mx_list_size(list);

	s_unread* senders_list = malloc(*senders_num * sizeof(s_unread));
	int i = 0;
	t_list* cur_list = list;
	while (cur_list != NULL) {
		s_unread* unread = cur_list->data;
		senders_list[i] = *unread;
		cur_list = cur_list->next;
		i++;
	}
	mx_t_list_deep_free(list);
	sqlite3_finalize(stmt);
	return senders_list;
}
