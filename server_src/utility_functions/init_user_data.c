#include "server.h"

user_t* init_user_data(int id, char *name, bool is_online) {
	user_t *user_data = (user_t*)malloc(sizeof(user_t));
    user_data->user_id = id;
    user_data->contacts_count = 0;
	user_data->contacts_id = malloc(0);
	user_data->groups_count = 0;
	user_data->groups_id = malloc(0);
	if (name != NULL) { 
		strcpy(user_data->name, name); 
	}
	else {
		printf("Warning init_user_data got no name in call\n");
		fflush(stdout);
	}
	user_data->is_online = is_online;

	return user_data;
}

