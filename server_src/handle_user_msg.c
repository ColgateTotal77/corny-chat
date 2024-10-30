#include "server.h"
#include "../libmx/inc/libmx.h"


void handle_send_to_command(call_data_t *call_data, char **params) {
    int chat_id = mx_atoi(params[1]);
    char *msg_to_send = params[2];

    send_message_to_chat(call_data, msg_to_send, chat_id);
    //printf("%s", "sended something to odd_chat\n");
	fflush(stdout);
}

void handle_create_chat_command(call_data_t *call_data, char **params) {
    int user_id = call_data->client_data->user_data->user_id;
	//char chat_name[32];
	//strncpy(chat_name, params[1], strlen(params[1]) - 1);

	chat_t *new_chat = (chat_t*)malloc(sizeof(chat_t));
    new_chat->users_id = (int*)malloc(0);
    new_chat->users_count = 0;
    new_chat->chat_id = *(call_data->chat_uid);
    *(call_data->chat_uid) = *(call_data->chat_uid) + 1;
    strncpy(new_chat->name, params[1], strlen(params[1]) - 1);

    append_to_intarr(&new_chat->users_id, &new_chat->users_count, user_id);
	ht_set(call_data->chats, new_chat->chat_id, (void*)new_chat);
}

void handle_see_all_chats_command(call_data_t *call_data) {
    char chats_info[BUF_SIZE];
    //char chat_name[32];
    int count = 0;
	entry_t** entries = ht_dump(call_data->chats, &count);

    int pos = 0;
    
    for (int i = 0; i < count; i++) {
        chat_t* chat_data = entries[i]->value;
        //printf("%lu\n", strlen(chat_data->name));
        //fflush(stdout);
        //strncpy(chat_name, chat_data->name, strlen(chat_data->name));
        pos += sprintf(&chats_info[pos], "%d.  %s\n", entries[i]->key, chat_data->name);
        //bzero(chat_name, 32);
    }

    free(entries);

    send_message_to_user(call_data, chats_info);
}

void handle_see_all_users_command(call_data_t *call_data) {
    char users_info[BUF_SIZE];
    char user_name[32];
    int count = 0;
	entry_t** entries = ht_dump(call_data->clients, &count);

    int pos = 0;
    
    for (int i = 0; i < count; i++) {
        entry_t* entry = entries[i];
        strcpy(user_name, ((client_t*)entry->value)->user_data->name);
        pos += sprintf(&users_info[pos], "%d.  %s\n", entry->key, user_name);
        memset(user_name, 0, sizeof(user_name));
    }

    free(entries);

    send_message_to_user(call_data, users_info);
}

void handle_add_contact_command(call_data_t *call_data, char **params) {
    int contact_id = mx_atoi(params[1]);
    int user_id = call_data->client_data->user_data->user_id;
    client_t *contact_data = ht_get(call_data->clients, contact_id);

    int **new_contact_contacts_list = &contact_data->user_data->contacts_id;
    int *new_contact_contacts_count = &contact_data->user_data->contacts_count;

    if (!num_inarray(*new_contact_contacts_list, *new_contact_contacts_count, user_id)) {
        append_to_intarr(new_contact_contacts_list, new_contact_contacts_count, user_id);
        char buffer[BUF_SIZE];
        sprintf(buffer, "%s added you to contacts\n", call_data->client_data->user_data->name);
        send_message_to_id(call_data, buffer, contact_id);
    }

    int **user_contacts_list = &call_data->client_data->user_data->contacts_id;
    int *user_contacts_count = &call_data->client_data->user_data->contacts_count;

    if (!num_inarray(*user_contacts_list, *user_contacts_count, contact_id)) {
        append_to_intarr(user_contacts_list, user_contacts_count, contact_id);
        char buffer[BUF_SIZE];
        sprintf(buffer, "You added %s to contacts\n", contact_data->user_data->name);
        send_message_to_user(call_data, buffer);
    }
    else {
        char buffer[BUF_SIZE];
        sprintf(buffer, "%s is already in your contacts\n", contact_data->user_data->name);
        send_message_to_user(call_data, buffer);
    }
}

void handle_see_my_contacts_command(call_data_t *call_data) {
    int *contacts_list = call_data->client_data->user_data->contacts_id;
    int contacts_count = call_data->client_data->user_data->contacts_count;

    char users_info[BUF_SIZE];
    char user_name[32];

    int pos = 0;
    
    for (int i = 0; i < contacts_count; i++) {
        client_t *client_data = ht_get(call_data->clients, contacts_list[i]);
        strcpy(user_name, client_data->user_data->name);
        pos += sprintf(&users_info[pos], "%d.  %s\n", contacts_list[i], user_name);
        memset(user_name, 0, sizeof(user_name));
    }

    send_message_to_user(call_data, users_info);
}

void handle_join_chat_command(call_data_t *call_data, char **params) {
    int chat_id = mx_atoi(params[1]);
    int user_id = call_data->client_data->user_data->user_id;

    chat_t *chat = ht_get(call_data->chats, chat_id);

    append_to_intarr(&chat->users_id, &chat->users_count, user_id);

    char buffer[BUF_SIZE];
    sprintf(buffer, "Succesfully joined to chat %s\n", chat->name);
    send_message_to_user(call_data, buffer);
}

void handle_add_contact_to_chat_command(call_data_t *call_data, char **params) {
    int contact_id = mx_atoi(params[1]);
    int chat_id = mx_atoi(params[2]);

    if (!num_inarray(call_data->client_data->user_data->contacts_id, 
                     call_data->client_data->user_data->contacts_count, contact_id)) {
        send_message_to_user(call_data, "No such contact\n");
        return;
    }

    chat_t *chat = ht_get(call_data->chats, chat_id);
    client_t *contact_data = ht_get(call_data->clients, contact_id);

    if (num_inarray(chat->users_id, chat->users_count, contact_id)) {
        char buffer[BUF_SIZE];
        sprintf(buffer, "%s already joined to chat %s\n", contact_data->user_data->name, chat->name);
        send_message_to_user(call_data, buffer);
        return;
    }

    append_to_intarr(&chat->users_id, &chat->users_count, contact_id);

    char buffer[BUF_SIZE];
    sprintf(buffer, "Succesfully added %s to chat %s\n", contact_data->user_data->name, chat->name);
    send_message_to_user(call_data, buffer);
}

void handle_msg_to_command(call_data_t *call_data, char **params) {
    int contact_id = mx_atoi(params[1]);
    char *msg_to_send = params[2];

    if (!num_inarray(call_data->client_data->user_data->contacts_id, 
                     call_data->client_data->user_data->contacts_count, contact_id)) {
        send_message_to_user(call_data, "No such contact\n");
        return;
    }

    char buffer[BUF_SIZE];
    sprintf(buffer, "%s (from private messages): %s", call_data->client_data->user_data->name, msg_to_send);
    send_message_to_id(call_data, buffer, contact_id);
}


void handle_user_valid_msg(char *client_msg, int *leave_flag, call_data_t *call_data) {
    char **name_and_msg = mx_strsplit(client_msg, ':');
	char *msg = (name_and_msg[1]) + 1;
    
    if (*msg != '/') {
        mx_del_strarr(&name_and_msg);
        send_message_to_another_ids(call_data, client_msg);
	    printf("%s -> %s\n", client_msg, call_data->client_data->user_data->name);
        return;
    }

    char **params = mx_strsplit(msg, ' ');
    bool handled_command = false;


	if (strcmp(params[0], "/send_to") == 0) {
		handle_send_to_command(call_data, params);
		handled_command = true;
	}

    if (!handled_command && strcmp(params[0], "/msg_to") == 0) {
		handle_msg_to_command(call_data, params);
		handled_command = true;
	}
	
	if (!handled_command && strcmp(params[0], "/create_chat") == 0) {
        handle_create_chat_command(call_data, params);
        handled_command = true;
	}

    if (!handled_command && strncmp(params[0], "/see_all_chats", 14) == 0) {
        handle_see_all_chats_command(call_data);
        handled_command = true;
	}

    if (!handled_command && strncmp(params[0], "/see_all_users", 14) == 0) {
        handle_see_all_users_command(call_data);
        handled_command = true;
	}

    if (!handled_command && strcmp(params[0], "/add_contact") == 0) {
        handle_add_contact_command(call_data, params);
        handled_command = true;
    }

    if (!handled_command && strncmp(params[0], "/see_my_contacts", 14) == 0) {
        handle_see_my_contacts_command(call_data);
        handled_command = true;
    }

    if (!handled_command && strcmp(params[0], "/join_chat") == 0) {
        handle_join_chat_command(call_data, params);
        handled_command = true;
    }

    if (!handled_command && strcmp(params[0], "/add_contact_to_chat") == 0) {
        handle_add_contact_to_chat_command(call_data, params);
        handled_command = true;
    }

    if (!handled_command && strncmp(params[0], "/exit", 5) == 0) {
        send_user_exit_msg(call_data);
        handled_command = true;
        *leave_flag = 1;
    }

    if (!handled_command) {
        send_message_to_another_ids(call_data, client_msg);
	    printf("%s -> %s\n", client_msg, call_data->client_data->user_data->name);
    }

    mx_del_strarr(&name_and_msg);
    mx_del_strarr(&params);
}


void handle_user_msg(int bytes_received, int *leave_flag, char *client_msg, call_data_t *call_data) {
    if (bytes_received > 0) {
		if (strlen(client_msg) <= 0) { return; }
		handle_user_valid_msg(client_msg, leave_flag, call_data);
	}
    else if (bytes_received == 0) {
        send_user_exit_msg(call_data);
		*leave_flag = 1;
	}
    else {
		printf("ERROR: -1\n");
		*leave_flag = 1;
	}
}


