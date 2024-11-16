#include "server.h"
#include "sql.h"


int create_new_user_and_return_id(call_data_t *call_data, char *login, unsigned char* password_hash) {
    user_create *usr = (user_create*)malloc(sizeof(user_create));
	init_user_create(usr, login, login, password_hash, 0);
    int user_id = create_user(call_data->general_data->db, *usr);

    free(usr);

    return user_id;
}
