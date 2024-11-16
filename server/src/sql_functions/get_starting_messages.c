//
// Created by konst on 11.11.24.
//

#include "../../inc/sql.h"

/**
 * @brief function returns array of texting when client connects to server
 * this array contains texting history between client and other clients that he has unread messages from
 * @attention use function <free_texting> to free memory correctly
 * @example
 *	int num = 0;
 *	s_texting* textings = get_starting_messages(db, 2, &num);
 *	free_texting(textings, num);
 *
 * @param db
 * @param user_id
 * @param senders_qty this param saves array length
 * @return s_texting[senders_qty] or NULL if user has no unread messages
 */
s_texting* get_starting_messages(sqlite3* db, const int user_id, int* senders_qty) {
	s_unread* senders = get_senders_list(db, user_id, senders_qty);
	s_texting* textings = NULL;
	if (*senders_qty > 0) {
		textings = malloc(sizeof(s_texting) * *senders_qty);
	}
	else {
		return NULL;
	}
	for (int i = 0; i < *senders_qty; i++) {
		const int sender = senders[i].sender_id;
		int mes_num = 0;
		s_message* mes_between = get_new_mess_between(db, user_id, sender, &mes_num);
		s_texting texting;
		texting.user1_id = user_id;
		texting.user2_id = sender;
		texting.all_mes_qty = mes_num;
		texting.unread_mes_qty = senders[i].unread_mes_qty;
		texting.messages = mes_between;
		textings[i] = texting;
	}

	free(senders);
	return textings;
}
