#include "server.h"

user_t* init_user_data(int id, char *login, char *nickname, bool is_online) {
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
	user_data->is_online = is_online;

	return user_data;
}

