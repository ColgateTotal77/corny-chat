#include "server.h"
#include "../libmx/inc/libmx.h"

void append_to_group_users(chat_t *chat_data, int user_id) {
    append_to_intarr(&chat_data->users_id, &chat_data->users_count, user_id);
}

void append_to_users_groups(user_t *user_data, int chat_id) {
    append_to_intarr(&user_data->groups_id, &user_data->groups_count, chat_id);
}

void append_to_users_contacts(user_t *user_data, int contact_id) {
    append_to_intarr(&user_data->contacts_id, &user_data->contacts_count, contact_id);
}

void remove_from_group_users(chat_t *chat_data, int user_id) {
    remove_from_intarr(&chat_data->users_id, &chat_data->users_count, user_id);
}

void remove_from_users_groups(user_t *user_data, int chat_id) {
    remove_from_intarr(&user_data->groups_id, &user_data->groups_count, chat_id);
}

void update_group_users_and_user_groups(chat_t *chat_data, client_t *client_data) {
    append_to_group_users(chat_data, client_data->user_data->user_id);
    append_to_users_groups(client_data->user_data, chat_data->chat_id);
}

bool user_exists(call_data_t *call_data, int user_id) {
    client_t *user_data = ht_get(call_data->general_data->clients, user_id);

    return user_data != NULL;
}

bool user_has_such_contact(user_t *user_data, int contact_id) {
    bool result = num_inarray(user_data->contacts_id, user_data->contacts_count, contact_id);

    return result;
}

bool group_has_such_user(chat_t *chat_data, int user_id) {
    bool result = num_inarray(chat_data->users_id, chat_data->users_count, user_id);

    return result;
}

bool is_user_group_owner(call_data_t *call_data, int group_id) {
    chat_t *chat_data = ht_get(call_data->general_data->chats, group_id);
    int user_id = call_data->client_data->user_data->user_id;

    return chat_data->owner_id == user_id;
}
