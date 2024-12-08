#include "create_json.h"

static void add_contacts_to_json(call_data_t *call_data, cJSON *response_json) {
    // Critical resource access: USER DATA. Start
    pthread_mutex_lock(&call_data->client_data->user_data->mutex);
    int contacts_count = call_data->client_data->user_data->contacts_count;

    cJSON *contacts_array = cJSON_AddArrayToObject(response_json, "contacts");
    cJSON_AddNumberToObject(response_json, "contacts_count", contacts_count);

    int *contacts_list = call_data->client_data->user_data->contacts_id;
    
    for (int i = 0; i < contacts_count; i++) {
        cJSON *contact_data_json = cJSON_CreateObject();

        // Critical resource access: CLIENTS HASH TABLE. Start
        pthread_mutex_lock(call_data->general_data->clients_mutex);
        client_t *client_data = ht_get(call_data->general_data->clients, contacts_list[i]);
        pthread_mutex_unlock(call_data->general_data->clients_mutex);
        // Critical resource access: CLIENTS HASH TABLE. End

        // Critical resource access: CLIENT USER DATA. Start
        pthread_mutex_lock(&client_data->user_data->mutex);
        cJSON_AddNumberToObject(contact_data_json, "id", client_data->user_data->user_id);
        cJSON_AddStringToObject(contact_data_json, "login", client_data->user_data->login);
        cJSON_AddStringToObject(contact_data_json, "nickname", client_data->user_data->nickname);
        cJSON_AddNumberToObject(contact_data_json, "online", client_data->user_data->is_online);
        cJSON_AddNumberToObject(contact_data_json, "admin", client_data->user_data->is_admin);
        cJSON_AddNumberToObject(contact_data_json, "active", client_data->user_data->is_active);
        pthread_mutex_unlock(&client_data->user_data->mutex);
        // Critical resource access: CLIENT USER DATA. End

        cJSON_AddItemToArray(contacts_array, contact_data_json);
    }
    pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
    // Critical resource access: USER DATA. End
}

static void add_groups_to_json(call_data_t *call_data, cJSON *response_json) {
    // Critical resource access: USER DATA. Start
    pthread_mutex_lock(&call_data->client_data->user_data->mutex);
    int groups_count = call_data->client_data->user_data->groups_count;

    cJSON *groups_array = cJSON_AddArrayToObject(response_json, "groups");
    cJSON_AddNumberToObject(response_json, "groups_count", groups_count);

    int *groups_list = call_data->client_data->user_data->groups_id;
    
    for (int i = 0; i < groups_count; i++) {
        cJSON *group_data_json = cJSON_CreateObject();
        chat_t *chat = NULL;
    
        // Critical resource access: SELECTED CHAT. Start
        bool chat_retrieved_successfully = get_group_data_and_lock_group_mutex(
            call_data, groups_list[i], &chat
        );
    
        if (!chat_retrieved_successfully) {
            cJSON_Delete(group_data_json);
            continue;
        }

        cJSON_AddNumberToObject(group_data_json, "id", chat->chat_id);
        cJSON_AddStringToObject(group_data_json, "name", chat->name);

        pthread_mutex_unlock(&chat->mutex);
        // Critical resource access: SELECTED CHAT. End

        cJSON_AddItemToArray(groups_array, group_data_json);
    }
    pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
    // Critical resource access: USER DATA. End
}

cJSON *handle_get_my_contacts(call_data_t *call_data) {
    cJSON *response_json = cJSON_CreateObject();
    
    add_contacts_to_json(call_data, response_json);
    add_groups_to_json(call_data, response_json);

    return response_json;
}
