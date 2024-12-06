#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "create_json.h"

bool get_int_arr_from_json(call_data_t* call_data, cJSON *json, chat_t *chat,
                           int **array_ptr, int *array_size) {
    int users_count = cJSON_GetArraySize(json);
    int *users_to_add = malloc(0);
    int users_num = 0;

    for (int i = 0; i < users_count; i++) {
        cJSON *user_id_json = cJSON_GetArrayItem(json, i);
        int user_id = (int)cJSON_GetNumberValue(user_id_json);

        if (num_inarray(users_to_add, users_num, user_id)
            || group_has_such_user(chat, user_id)
            || !user_exists(call_data, user_id)) {

            free(users_to_add);
            return false;
        }

        append_to_intarr(&users_to_add, &users_num, user_id);
    }

    *array_ptr = users_to_add;
    *array_size = users_num;

    return true;
}


cJSON *handle_add_many_users_to_group(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "users_id")
        || !cJSON_HasObjectItem(json, "chat_id")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *users_id_json = cJSON_GetObjectItemCaseSensitive(json, "users_id");
    if (!cJSON_IsArray(users_id_json)) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);

    chat_t *chat = NULL;
    
    // Critical resource access: SELECTED CHAT. Start
    bool chat_retrieved_successfully = get_group_data_and_lock_group_mutex(
        call_data, chat_id, &chat
    );

    if (!chat_retrieved_successfully) {
        return create_error_json("No such chat\n");
    }

    if (chat->owner_id != call_data->client_data->user_data->user_id) {
        pthread_mutex_unlock(&chat->mutex);
        // Critical resource access: SELECTED CHAT. Possible end

        return create_error_json("You have to rights\n");
    }

    int *users_to_add = NULL;
    int users_num = 0;

    // Critical resource access: CLIENTS HASH TABLE. Start
    pthread_mutex_lock(call_data->general_data->clients_mutex);
    if (!get_int_arr_from_json(call_data, users_id_json, chat, &users_to_add, &users_num)) {
        pthread_mutex_unlock(call_data->general_data->clients_mutex);
        // Critical resource access: CLIENTS HASH TABLE. Possible end

        pthread_mutex_unlock(&chat->mutex);
        // Critical resource access: SELECTED CHAT. Possible end

        return create_error_json("Invalid json format\n");
    }
    
    // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
    add_users_to_group(call_data->general_data->db, chat_id, users_to_add, users_num);
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End

    // Critical resource access: USER DATA. Start
    pthread_mutex_lock(&call_data->client_data->user_data->mutex);

    cJSON *notification_json = you_were_added_to_group_notification( 
        call_data->client_data->user_data->user_id,
        call_data->client_data->user_data->nickname,
        chat_id,
        chat->name
    );

    pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
    // Critical resource access: USER DATA. End

    cJSON *users_array = cJSON_CreateArray();

    for (int i = 0; i < users_num; i++) {
        client_t *client_data = ht_get(call_data->general_data->clients, users_to_add[i]);

        cJSON *user_data = cJSON_CreateObject();

        // Critical resource access: CLIENT USER DATA. Start
        pthread_mutex_lock(&client_data->user_data->mutex);
        update_group_users_and_user_groups(chat, client_data);
        cJSON_AddNumberToObject(user_data, "id", client_data->user_data->user_id);
        cJSON_AddStringToObject(user_data, "nickname", client_data->user_data->nickname);
        pthread_mutex_unlock(&client_data->user_data->mutex);
        // Critical resource access: CLIENT USER DATA. End

        cJSON_AddItemToArray(users_array, user_data);
        
        send_json_to_client(&notification_json, client_data);
        
    }
    cJSON_Delete(notification_json);

    pthread_mutex_unlock(call_data->general_data->clients_mutex);
    // Critical resource access: CLIENTS HASH TABLE. End

    pthread_mutex_unlock(&chat->mutex);
    // Critical resource access: SELECTED CHAT. End

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddItemToObject(response_json, "users", users_array);
    cJSON_AddNumberToObject(response_json, "chat_id", chat_id);

    free(users_to_add);

    return response_json;
}

