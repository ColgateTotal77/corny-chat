#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "create_json.h"


cJSON *handle_join_chat(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "chat_id")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);
    int user_id = call_data->client_data->user_data->user_id;
    chat_t *chat = NULL;
    
    // Critical resource access: SELECTED CHAT. Start
    bool chat_retrieved_successfully = get_group_data_and_lock_group_mutex(
        call_data, chat_id, &chat
    );

    if (!chat_retrieved_successfully) {
        cJSON *error_response = create_error_json("No such chat\n");
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    if (group_has_such_user(chat, user_id)) {
         pthread_mutex_unlock(&chat->mutex);
        // Critical resource access: SELECTED CHAT. Possible end

        cJSON *error_response = create_error_json("Already joined\n");
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    const int users[] = {user_id};

    // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
	int res = add_users_to_group(call_data->general_data->db, chat_id, users, 1);
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End

    if (res <= 0) {
        pthread_mutex_unlock(&chat->mutex);
        // Critical resource access: SELECTED CHAT. Possible end

        cJSON *error_response = create_error_json("Something went wrong\n");
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    append_to_group_users(chat, user_id);
    pthread_mutex_unlock(&chat->mutex);
    // Critical resource access: SELECTED CHAT. End

    // Critical resource access: USER DATA. Start
    pthread_mutex_lock(&call_data->client_data->user_data->mutex);
    append_to_users_groups(call_data->client_data->user_data, chat_id);
    pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
    // Critical resource access: USER DATA. End

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "chat_joined", chat_id);
    cJSON_AddStringToObject(response_json, "chat_name", chat->name);

    return response_json;
}

