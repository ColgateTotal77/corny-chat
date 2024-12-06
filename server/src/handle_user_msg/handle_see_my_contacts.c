#include "server.h"


cJSON *handle_see_my_contacts(call_data_t *call_data) {
    printf("MUTEX LOG: handle_see_my_contacts<--------------------------------\n");
    fflush(stdout);

    printf("MUTEX LOG: lock(&call_data->client_data->user_data->mutex)\n");
    fflush(stdout);
    // Critical resource access: USER DATA. Start
    pthread_mutex_lock(&call_data->client_data->user_data->mutex);
    int contacts_count = call_data->client_data->user_data->contacts_count;
    int *contacts_list = call_data->client_data->user_data->contacts_id;

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "contacts_count", contacts_count);
    cJSON *contacts_json_array = cJSON_AddArrayToObject(response_json, "contacts");
    
    for (int i = 0; i < contacts_count; i++) {
        cJSON *contact_data_json = cJSON_CreateObject();

        printf("MUTEX LOG: lock(call_data->general_data->clients_mutex)\n");
        fflush(stdout);
        // Critical resource access: CLIENTS HASH TABLE. Start
        pthread_mutex_lock(call_data->general_data->clients_mutex);
        client_t *client_data = ht_get(call_data->general_data->clients, contacts_list[i]);
        pthread_mutex_unlock(call_data->general_data->clients_mutex);
        // Critical resource access: CLIENTS HASH TABLE. End
        printf("MUTEX LOG: unlock(call_data->general_data->clients_mutex)\n");
        fflush(stdout);

        printf("MUTEX LOG: lock(&client_data->user_data->mutex)\n");
        fflush(stdout);
        // Critical resource access: CLIENT USER DATA. Start
        pthread_mutex_lock(&client_data->user_data->mutex);
        cJSON_AddNumberToObject(contact_data_json, "id", contacts_list[i]);
        cJSON_AddStringToObject(contact_data_json, "login", client_data->user_data->login);
        cJSON_AddStringToObject(contact_data_json, "nickname", client_data->user_data->nickname);
        cJSON_AddBoolToObject(contact_data_json, "online", client_data->user_data->is_online);
        cJSON_AddBoolToObject(contact_data_json, "admin", client_data->user_data->is_admin);
        cJSON_AddBoolToObject(contact_data_json, "active", client_data->user_data->is_active);
        pthread_mutex_unlock(&client_data->user_data->mutex);
        // Critical resource access: CLIENT USER DATA. End
        printf("MUTEX LOG: unlock(&client_data->user_data->mutex)\n");
        fflush(stdout);

        cJSON_AddItemToArray(contacts_json_array, contact_data_json);
    }

    pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
    // Critical resource access: USER DATA. End
    printf("MUTEX LOG: unlock(&call_data->client_data->user_data->mutex)\n");
    fflush(stdout);

    return response_json;
}

