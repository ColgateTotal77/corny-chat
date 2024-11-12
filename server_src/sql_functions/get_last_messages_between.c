//
// Created by konst on 11.11.24.
//


#include "../../libmx_konst/inc/libmx.h"
#include "../../inc/sql.h"

/**
 * @brief message array can be NULL if users don't have texting yet.
 * it also can be seen in structure parameter <all_mes_qty> = 00
 * if before == 0 message array contains qty of last messages
 * if before == message ID array contains qty of last messages before message ID
 * @warning function allocates memory. Use free_texting func with 1 as second parameter
 * @example
 * 	s_texting* texting = get_last_messages_between(db, 1,2,25);
 *	free_texting(texting, 1);
 *
 * @param db
 * @param usr1_id
 * @param usr2_id
 * @param qty max message's number that will be added to array
 * @param before must be <0> or the earliest sent message_ID
 * @return pointer to structure s_texting with sorted message history between 2 users
 * or NULL if something went wrong
 */
s_texting* get_last_messages_between(sqlite3* db, const int usr1_id, const int usr2_id,
                                     const int qty, const int before) {
	const char* sql = NULL;
	if (before <= 0) {
		sql = "SELECT id, createdAt, ownerId, targetUserId, targetGroupId, message, readed "
			"FROM (SELECT * "
			"FROM messages m "
			"WHERE m.ownerId IN (?, ?) AND  m.targetUserId IN (?, ?) "
			"ORDER BY m.createdAt DESC "
			"LIMIT ?) "
			"ORDER BY createdAt;";
	}
	else {
		sql = "SELECT id, createdAt, ownerId, targetUserId, targetGroupId, message, readed "
			"FROM (SELECT * "
			"FROM messages m "
			"WHERE m.ownerId IN (?, ?) AND  m.targetUserId IN (?, ?) AND m.id < ?"
			"ORDER BY m.createdAt DESC "
			"LIMIT ?) "
			"ORDER BY createdAt;";
	}
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
	if (before <= 0) {
		sqlite3_bind_int(stmt, 5, qty);
	} else {
		sqlite3_bind_int(stmt, 5, before);
		sqlite3_bind_int(stmt, 6, qty);
	}

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
	const int real_qty = mx_list_size(list);
	s_message* messages = NULL;
	if (real_qty > 0) {
		messages = malloc(sizeof(s_message) * real_qty);
		if (!messages) return NULL;
		t_list* temp_list = list;
		int i = 0;
		while (temp_list) {
			s_message* message = temp_list->data;
			messages[i] = *message;
			temp_list = temp_list->next;
			i++;
		}
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
