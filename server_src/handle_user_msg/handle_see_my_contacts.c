#include "server.h"


cJSON *handle_see_my_contacts(call_data_t *call_data) {
    int contacts_count = call_data->client_data->user_data->contacts_count;
    int *contacts_list = call_data->client_data->user_data->contacts_id;

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "contacts_count", contacts_count);
    cJSON *contacts_json_array = cJSON_AddArrayToObject(response_json, "contacts");
    
    for (int i = 0; i < contacts_count; i++) {
        client_t *client_data = ht_get(call_data->general_data->clients, contacts_list[i]);

        cJSON *contact_data_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(contact_data_json, "id", contacts_list[i]);
        cJSON_AddStringToObject(contact_data_json, "name", client_data->user_data->name);
        cJSON_AddBoolToObject(contact_data_json, "online", client_data->user_data->is_online);

        cJSON_AddItemToArray(contacts_json_array, contact_data_json);
    }

    return response_json;
}

