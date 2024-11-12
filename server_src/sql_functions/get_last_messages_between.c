//
// Created by konst on 11.11.24.
//

#include <tlist.h>

#include "libmx.h"
#include "../../inc/sql.h"

void print_message(const s_message *msg);
void print_texting(const s_texting* texting);


s_texting* get_last_messages_between(sqlite3* db, const int usr1_id, const int usr2_id, const int qty) {
	const char* sql = "SELECT id, createdAt, ownerId, targetUserId, targetGroupId, message, readed "
		"FROM (SELECT * "
		"FROM messages m "
		"WHERE m.ownerId IN (?, ?) AND  m.targetUserId IN (?, ?) "
		"ORDER BY m.createdAt DESC "
		"LIMIT ?) "
		"ORDER BY createdAt;";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK) {
		printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return NULL;
	}

	sqlite3_bind_int(stmt, 1, usr1_id);
	sqlite3_bind_int(stmt, 2, usr2_id);
	sqlite3_bind_int(stmt, 3, usr1_id);
	sqlite3_bind_int(stmt, 4, usr2_id);
	sqlite3_bind_int(stmt, 5, qty);

	t_list* list = NULL;
	int unread_num = 0;
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		s_message* message = malloc(sizeof(s_message));
		init_message(message,
			 sqlite3_column_int(stmt, 0),
			 (const char*)sqlite3_column_text(stmt, 1),
			 sqlite3_column_int(stmt, 2),
			 sqlite3_column_int(stmt, 3),
			 sqlite3_column_int(stmt, 4),
			 (char*)sqlite3_column_text(stmt, 5),
			 (bool)sqlite3_column_int(stmt, 6));
		if (!message->readed) unread_num++;
		mx_push_back(&list, message);
	}
	// fprintf(stderr, "sqlite rc =  %s\n", sqlite3_errmsg(db));
	const int real_qty = mx_list_size(list);
	s_message* messages = malloc(sizeof(s_message) * real_qty);

	t_list* temp_list = list;
	int i = 0;
	while (temp_list) {
		s_message* message = temp_list->data;
		messages[i] = *message;
		temp_list = temp_list->next;
		i++;
	}
	mx_t_list_deep_free(list);
	s_texting* texting = malloc(sizeof(s_texting));
	texting->user1_id = usr1_id;
	texting->user2_id = usr2_id;
	texting->all_mes_qty = real_qty;
	texting->unread_mes_qty = unread_num;
	texting->messages = messages;

	sqlite3_finalize(stmt);
	return texting;
}
