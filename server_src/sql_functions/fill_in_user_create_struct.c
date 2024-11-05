//
// Created by konstantin on 06.11.24.
//
#include "../../inc/sql.h"


void init_user_create(user_create *usr, const char *login, const char *nickname,
                      const unsigned char *pas_hash, const int role_id) {
    snprintf(usr->login, sizeof(usr->login), "%s", login);
    snprintf(usr->nickname, sizeof(usr->nickname), "%s", nickname?nickname:login);
    memcpy(usr->password, pas_hash, sizeof(usr->password));
    usr->role_id = role_id;
}
