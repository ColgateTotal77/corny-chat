#include "server.h"

static void add_contacts_to_json(call_data_t *call_data, cJSON *response_json) {
    int contacts_count = call_data->client_data->user_data->contacts_count;

    cJSON *contacts_array = cJSON_AddArrayToObject(response_json, "contacts");
    cJSON_AddNumberToObject(response_json, "contacts_count", contacts_count);

    int *contacts_list = call_data->client_data->user_data->contacts_id;
    
    for (int i = 0; i < contacts_count; i++) {
        client_t *client_data = ht_get(call_data->general_data->clients, contacts_list[i]);
        cJSON *contact_data_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(contact_data_json, "id", client_data->user_data->user_id);
        cJSON_AddStringToObject(contact_data_json, "nickname", client_data->user_data->name);
        cJSON_AddNumberToObject(contact_data_json, "online", client_data->user_data->is_online);
        cJSON_AddItemToArray(contacts_array, contact_data_json);
    }
}

static void add_groups_to_json(call_data_t *call_data, cJSON *response_json) {
    int groups_count = call_data->client_data->user_data->groups_count;

    cJSON *groups_array = cJSON_AddArrayToObject(response_json, "groups");
    cJSON_AddNumberToObject(response_json, "groups_count", groups_count);

    int *groups_list = call_data->client_data->user_data->groups_id;
    
    for (int i = 0; i < groups_count; i++) {
        chat_t *chat_data = ht_get(call_data->general_data->chats, groups_list[i]);
        cJSON *group_data_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(group_data_json, "id", chat_data->chat_id);
        cJSON_AddStringToObject(group_data_json, "name", chat_data->name);
        cJSON_AddItemToArray(groups_array, group_data_json);
    }
}

cJSON *handle_get_my_contacts(call_data_t *call_data) {
    cJSON *response_json = cJSON_CreateObject();
    
    add_contacts_to_json(call_data, response_json);
    add_groups_to_json(call_data, response_json);

    return response_json;
}
