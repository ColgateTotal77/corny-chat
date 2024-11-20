#include "server.h"
#include "sql.h"
#include "../libmx/inc/libmx.h"

static void set_user_contacts(sqlite3 *db, int user_id, int **contacts_list, int *contacts_count) {
	int chat_users_num = 0;
	chat_user* chat_users = get_clients_userslist(db, user_id, false, &chat_users_num);

    if (chat_users == NULL) {
        return;
    }

    for (int i = 0; i < chat_users_num; i++) {
        append_to_intarr(contacts_list, contacts_count, chat_users[i].id);
    }
	free(chat_users);
}

user_t* init_user_data(sqlite3 *db, int id, char *login, char *nickname, bool is_online) {
	user_t *user_data = (user_t*)malloc(sizeof(user_t));
    user_data->user_id = id;
    user_data->contacts_count = 0;
	user_data->contacts_id = malloc(0);
	user_data->groups_count = 0;
	user_data->groups_id = malloc(0);
	if (login != NULL) { 
		strcpy(user_data->login, login);  
	}
	else {
		printf("Warning init_user_data got no login in call\n");
		fflush(stdout);
	}

	if (nickname != NULL) { 
		strcpy(user_data->nickname, nickname);  
	}
	else {
		if (login != NULL) { strcpy(user_data->nickname, login); }
		printf("Warning init_user_data got no nickname in call\n");
		fflush(stdout);
	}
	set_user_contacts(db, user_data->user_id, &user_data->contacts_id, &user_data->contacts_count);
	user_data->is_online = is_online;

	return user_data;
}

