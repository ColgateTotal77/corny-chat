#include "server.h"
#include "sql.h"
#include "../libmx/inc/libmx.h"

static sqlite3 *open_db_connection(bool *stop_server) {
    sqlite3 *db; //connection variable

	int rc = sqlite3_open("db/uchat_db.db", &db); //open DB connection
    //check is connect successful
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
		*(stop_server) = true;
    }
	else {
	    printf("connection to db opened\n");
	}

	 char* errmsg = NULL;
	 sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, &errmsg);
	 if (errmsg) {
	 	fprintf(stderr, "Error: %s\n", errmsg);
//	 	sqlite3_free(errmsg);  // Освобождение памяти
//	 	sqlite3_close(db);
	 }

    return db;
}

static ht_t *load_users_from_db_to_hash_table(sqlite3 *db, ht_str_t **login_to_id) {
    *(login_to_id) = ht_str_create();
	ht_t *clients = ht_create();
	s_user user_db_data;
	
    int user_quantity = get_usr_qty(db);
	s_user *users_db_info = select_all_users(db);

	for (int i = 0; i < user_quantity; i++) {
		user_db_data = users_db_info[i];

        add_offline_user_to_server_cache(
            db, clients, *login_to_id,
            user_db_data.id, user_db_data.login,
            user_db_data.nickname, 
            (user_db_data.role_id == 1),
            user_db_data.active);
	}
	
	free(users_db_info);

    return clients;
}

static void update_user_groups_data(ht_t *clients_table, int group_id, int user_id) {
    client_t *client_data = ht_get(clients_table, user_id);
    append_to_users_groups(client_data->user_data, group_id);
}

static chat_t *init_new_group_and_update_users_data(s_group group_data, ht_t *clients_table) {
    chat_t *group = (chat_t*)malloc(sizeof(chat_t));
    group->chat_id = group_data.id;
    group->owner_id = group_data.owner_id;
    bzero(group->name, 50);
    strcpy(group->name, group_data.name);
    bzero(group->createdAt, 20);
    strcpy(group->createdAt, group_data.createdAt);
    group->users_count = 0;
    group->users_id = malloc(0);

    for (int i = 0; i < group_data.occupants_num; i++) {
        int occupant_id = group_data.occupants[i];
        append_to_group_users(group, occupant_id);
        update_user_groups_data(clients_table, group_data.id, occupant_id);
    }

    return group;
}

static ht_t *load_groups_from_db_to_hash_table(sqlite3 *db, ht_t *clients_table) {
    ht_t *chats = ht_create();

    int groups_num = 0;
    s_group *all_groups = get_groups_full_list(db, 0, &groups_num);

    for (int i = 0; i < groups_num; i++) {
        chat_t *group = init_new_group_and_update_users_data(all_groups[i], clients_table);
        ht_set(chats, group->chat_id, (void*)group);
    }

    free_groups_full_list(all_groups, groups_num);

    return chats;
}


general_data_t *setup_general_data(bool *stop_server, int *online_count) {
    general_data_t *general_data = (general_data_t*)malloc(sizeof(general_data_t));
    general_data->db = open_db_connection(stop_server);
    general_data->login_to_id = NULL;
    general_data->session_id_to_id = ht_str_create();
    general_data->clients = load_users_from_db_to_hash_table(general_data->db,
                                                             &general_data->login_to_id);
    general_data->chats = load_groups_from_db_to_hash_table(general_data->db, 
                                                            general_data->clients);
	general_data->online_count = online_count;


    return general_data;
}



