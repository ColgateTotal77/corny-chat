#include "server.h"

void add_offline_user_to_server_cache(sqlite3 *db, ht_t *clients, ht_str_t *login_to_id,
                                      int user_id, char *login, char *nickname) {
    user_t *user_data;
    if (!nickname) {
        user_data = init_user_data(db, user_id, login, login, false);
    }
    else {
        user_data = init_user_data(db, user_id, login, nickname, false);
    }

    client_t *client_data = (client_t*)malloc(sizeof(client_t));
	//client_data->sockfd = -1;
    client_data->ssl = NULL;
	client_data->user_data = user_data;

	ht_set(clients, user_data->user_id, (void*)client_data);
	ht_str_set(login_to_id, login, user_id);
}
