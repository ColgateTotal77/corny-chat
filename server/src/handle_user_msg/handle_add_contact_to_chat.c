#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "create_json.h"

static cJSON *create_error_response(char *error_msg, int contact_id, int chat_id) {
    cJSON *error_response = create_error_json(error_msg);
    cJSON_AddNumberToObject(error_response, "contact_id", contact_id);
    cJSON_AddNumberToObject(error_response, "chat_id", chat_id);

    return error_response;
}

cJSON *handle_add_contact_to_chat(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "contact_id")
        || !cJSON_HasObjectItem(json, "chat_id")) {
        return create_error_json("Invalid json format\n");
    }
    
    cJSON *contact_id_json = cJSON_GetObjectItemCaseSensitive(json, "contact_id");
    int contact_id = (int)cJSON_GetNumberValue(contact_id_json);
    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);

    // Critical resource access: USER DATA. Start
    pthread_mutex_lock(&call_data->client_data->user_data->mutex);
    bool has_such_contact = user_has_such_contact(call_data->client_data->user_data, contact_id);
    pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
    // Critical resource access: USER DATA. End

    if (!has_such_contact) {
        return create_error_response("No such contact\n", contact_id, chat_id);
    }

    chat_t *chat = NULL;
    
    // Critical resource access: SELECTED CHAT. Start
    bool chat_retrieved_successfully = get_group_data_and_lock_group_mutex(
        call_data, chat_id, &chat
    );

    if (!chat_retrieved_successfully) {
        return create_error_response("No such group\n", contact_id, chat_id);
    }

    if (group_has_such_user(chat, contact_id)) {
        pthread_mutex_unlock(&chat->mutex);
        // Critical resource access: SELECTED CHAT. Possible end

        return create_error_response("This contact already joined a chat\n", contact_id, chat_id);
    }

    const int users[] = {contact_id};

	// Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
	int res = add_users_to_group(call_data->general_data->db, chat_id, users, 1);
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End

    if (res <= 0) {
        pthread_mutex_unlock(&chat->mutex);
        // Critical resource access: SELECTED CHAT. Possible end

        return create_error_response("Something went wrong\n", contact_id, chat_id);
    }
    
    // Critical resource access: USER DATA. Start
    pthread_mutex_lock(&call_data->client_data->user_data->mutex);
    cJSON *notification_json = you_were_added_to_group_notification( 
        call_data->client_data->user_data->user_id,
        call_data->client_data->user_data->nickname,
        chat_id, chat->name
    );
    pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
    // Critical resource access: USER DATA. End

    pthread_mutex_unlock(&chat->mutex);
    // Critical resource access: SELECTED CHAT. End

    // Critical resource access: CLIENTS HASH TABLE. Start
    pthread_mutex_lock(call_data->general_data->clients_mutex);
    client_t *contact_data = ht_get(call_data->general_data->clients, contact_id);
    pthread_mutex_unlock(call_data->general_data->clients_mutex);
    // Critical resource access: CLIENTS HASH TABLE. End

    // Critical resource access: CONTACT USER DATA. Start
    pthread_mutex_lock(&contact_data->user_data->mutex);
    update_group_users_and_user_groups(chat, contact_data);
    pthread_mutex_unlock(&contact_data->user_data->mutex);
    // Critical resource access: CONTACT USER DATA. End
    
    send_to_client_and_delete_json(&notification_json, contact_data);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "contact_id", contact_id);
    cJSON_AddNumberToObject(response_json, "chat_id", chat_id);

    return response_json;
}

