#include "server.h"

void add_offline_user_to_server_cache(sqlite3 *db, ht_t *clients, ht_str_t *login_to_id,
                                      int user_id, char *login, char *nickname, 
                                      bool is_admin, bool is_active) {
    user_t *user_data;
    user_data = init_user_data(db, user_id, login, 
                               (nickname == NULL) ? login : nickname,
                               is_admin, is_active, false);

    client_t *client_data = (client_t*)malloc(sizeof(client_t));
	//client_data->sockfd = -1;
    client_data->ssl = NULL;
	client_data->user_data = user_data;

	ht_set(clients, user_data->user_id, (void*)client_data);
	ht_str_set(login_to_id, login, user_id);
}
