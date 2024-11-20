#include "server.h"
#include "sql.h"

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

        add_offline_user_to_server_cache(db, clients, *login_to_id,
                                         user_db_data.id, user_db_data.login, user_db_data.nickname);
		
	}
	
	free(users_db_info);

    return clients;
}

static ht_t *initialize_chats_hashMap_and_set_up_default_chats(int *chat_uid) {
    ht_t *chats = ht_create();

    chat_t *odd_chat = (chat_t *)malloc(sizeof(chat_t));
    odd_chat->users_id = (int*)malloc(0);
    odd_chat->users_count = 0;
    odd_chat->chat_id = *chat_uid;
    *(chat_uid) = *(chat_uid) + 1;
    strcpy(odd_chat->name, "odd_chat");

	ht_set(chats, odd_chat->chat_id, (void*)odd_chat);

    return chats;
}


general_data_t *setup_general_data(bool *stop_server, int *online_count, int *chat_uid) {
    general_data_t *general_data = (general_data_t*)malloc(sizeof(general_data_t));
    general_data->db = open_db_connection(stop_server);
    general_data->login_to_id = NULL;
    general_data->session_id_to_id = ht_str_create();
    general_data->clients = load_users_from_db_to_hash_table(general_data->db,
                                                             &general_data->login_to_id);
    general_data->chat_uid = chat_uid;
    general_data->chats = initialize_chats_hashMap_and_set_up_default_chats(chat_uid);
	general_data->online_count = online_count;

    return general_data;
}



