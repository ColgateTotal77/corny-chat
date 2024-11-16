//
// Created by konstantin on 04.11.24.
//

#include "../../inc/sql.h"

/**
 *@brief fll in user structure
 *
 * @param usr structure
 * @param id int
 * @param created_at char*
 * @param login char*
 * @param nickname char*
 * @param role_id int
 * @param active bool
 */
void init_user(s_user *usr, const int id, const char *created_at, const char *login,
               const char *nickname,
               const int role_id, const bool active) {
    usr->id = id;
    snprintf(usr->createdAt, sizeof(usr->createdAt), "%s", created_at);
    snprintf(usr->login, sizeof(usr->login), "%s", login);
    snprintf(usr->nickname, sizeof(usr->nickname), "%s", nickname);
    usr->role_id = role_id;
    usr->active = active;
}
