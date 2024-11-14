//
// Created by konst on 14.11.24.
//

#include <stdio.h>
#include "../../inc/sql.h"

void init_chat_user(chat_user* usr, const int id, const char* nickname, const bool active,
                    const int unread_mes_qty) {
	usr->id = id;
	snprintf(usr->nickname, sizeof(usr->nickname), "%s", nickname);
	usr->active = active;
	usr->unread_mes_qty = unread_mes_qty; // -1 if there is NO messaging between
}
