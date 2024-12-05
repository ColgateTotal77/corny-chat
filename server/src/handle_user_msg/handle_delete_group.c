#include "create_json.h"
#include "../../libmx/inc/libmx.h"
#include "command_codes.h"


static void free_server_group(chat_t **chat_data) {
    pthread_mutex_destroy(&(*chat_data)->mutex);
    free((*chat_data)->users_id);
    free(*chat_data);
}

static cJSON *group_was_deleted_notification(int group_id) {
    cJSON *notification_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(notification_json, "event_code", GROUP_WAS_DELETED);
    cJSON_AddNumberToObject(notification_json, "group_id", group_id);

    return notification_json;
}

cJSON *handle_delete_group(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "group_id")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *group_id_json = cJSON_GetObjectItemCaseSensitive(json, "group_id");
    int user_id = call_data->client_data->user_data->user_id;

    // Critical resource access: CHATS HASH TABLE. Start
    pthread_mutex_lock(call_data->general_data->chats_mutex);
    chat_t *chat_data = ht_get(call_data->general_data->chats, group_id_json->valueint);

    if (NULL == chat_data) {
        pthread_mutex_unlock(call_data->general_data->chats_mutex);
        // Critical resource access: CHATS HASH TABLE. Possible end
    }

    // Critical resource access: SELECTED CHAT DATA. Start
    pthread_mutex_lock(&chat_data->mutex);

    if (user_id != chat_data->owner_id) {
        pthread_mutex_unlock(&chat_data->mutex);
        // Critical resource access: SELECTED CHAT DATA. Possible end

        pthread_mutex_unlock(call_data->general_data->chats_mutex);
        // Critical resource access: CHATS HASH TABLE. Possible end

        return create_error_json("You have to rights\n");
    }

    // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
    int rc = delete_group(call_data->general_data->db, group_id_json->valueint);
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End

    if (rc < 0) {
        pthread_mutex_unlock(&chat_data->mutex);
        // Critical resource access: SELECTED CHAT DATA. Possible end

        pthread_mutex_unlock(call_data->general_data->chats_mutex);
        // Critical resource access: CHATS HASH TABLE. Possible end

        return create_error_json("Something went wrong\n");
    }

    cJSON *group_was_deleted_notif = group_was_deleted_notification(chat_data->chat_id);

    send_to_group_and_delete_json(call_data, &group_was_deleted_notif, chat_data);

    for (int i = 0; i < chat_data->users_count; i++) {
        // Critical resource access: CLIENTS HASH TABLE. Start
        pthread_mutex_lock(call_data->general_data->clients_mutex);
        client_t *client_data = ht_get(call_data->general_data->clients, chat_data->users_id[i]);
        pthread_mutex_unlock(call_data->general_data->clients_mutex);
        // Critical resource access: CLIENTS HASH TABLE. End

        // Critical resource access: CLIENT USER DATA. Start
        pthread_mutex_lock(&client_data->user_data->mutex);
        remove_from_users_groups(client_data->user_data, chat_data->chat_id);
        pthread_mutex_unlock(&client_data->user_data->mutex);
        // Critical resource access: CLIENT USER DATA. End
    }

    free_server_group(&chat_data);
    // Critical resource access: SELECTED CHAT DATA. End

    ht_del(call_data->general_data->chats, group_id_json->valueint);
    pthread_mutex_unlock(call_data->general_data->chats_mutex);
    // Critical resource access: CHATS HASH TABLE. End
        

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "chat_id", group_id_json->valueint);

    return response_json;
}

