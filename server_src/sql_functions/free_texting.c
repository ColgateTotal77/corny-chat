//
// Created by konst on 11.11.24.
//

#include "../../inc/sql.h"

/**
 * 
 * @param textings 
 * @param senders_qty 
 */
void free_texting(s_texting* textings, const int senders_qty) {
	if (textings == NULL || senders_qty <= 0) {
		return;
	}
	for (int i = 0; i < senders_qty; i++) {
		free(textings[i].messages);
	}
	free(textings);
}
