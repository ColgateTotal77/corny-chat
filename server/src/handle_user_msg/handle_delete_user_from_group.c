#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "create_json.h"
#include "command_codes.h"

static cJSON *notif_you_were_deleted_from_group(int chat_id, char* chat_name) {
    cJSON *notification_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(notification_json, "event_code", YOU_WERE_DELETED_FROM_GROUP);
    cJSON_AddNumberToObject(notification_json, "chat_id", chat_id);
    cJSON_AddStringToObject(notification_json, "chat_name", chat_name);

    return notification_json;
}

static cJSON *notif_user_was_deleted_from_group(int chat_id, char* chat_name, 
                                          int user_id, char* user_nickname) {
    cJSON *notification_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(notification_json, "event_code", USER_WAS_DELETED_FROM_GROUP);
    cJSON_AddNumberToObject(notification_json, "chat_id", chat_id);
    cJSON_AddStringToObject(notification_json, "chat_name", chat_name);
    cJSON_AddNumberToObject(notification_json, "user_id", user_id);
    cJSON_AddStringToObject(notification_json, "user_nickname", user_nickname);

    return notification_json;
}

static cJSON *create_error_response_json(char *error_msg, int user_id, int chat_id) {
    cJSON *error_response = create_error_json(error_msg);
    cJSON_AddNumberToObject(error_response, "user_id", user_id);
    cJSON_AddNumberToObject(error_response, "chat_id", chat_id);

    return error_response;
}

cJSON *handle_delete_user_from_group(call_data_t *call_data, cJSON *json) {
    printf("MUTEX LOG: handle_delete_user_from_group<--------------------------------\n");
    fflush(stdout);
    if (!cJSON_HasObjectItem(json, "user_id")
        || !cJSON_HasObjectItem(json, "chat_id")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *user_to_delete_id_json = cJSON_GetObjectItemCaseSensitive(json, "user_id");
    int user_to_delete_id = (int)cJSON_GetNumberValue(user_to_delete_id_json);
    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);

    chat_t *chat = NULL;
    
    // Critical resource access: SELECTED CHAT. Start
    bool chat_retrieved_successfully = get_group_data_and_lock_group_mutex(
        call_data, chat_id, &chat
    );

    if (!chat_retrieved_successfully) {
        return create_error_response_json("No such group\n", user_to_delete_id, chat_id);
    }

    if (!group_has_such_user(chat, call_data->client_data->user_data->user_id)) {
        pthread_mutex_unlock(&chat->mutex);
        // Critical resource access: SELECTED CHAT. Possible end
        printf("MUTEX LOG: unlock(&chat->mutex)\n");
        fflush(stdout);

        return create_error_response_json("No such group\n", user_to_delete_id, chat_id);
    }

    if (chat->owner_id != call_data->client_data->user_data->user_id) {
        pthread_mutex_unlock(&chat->mutex);
        // Critical resource access: SELECTED CHAT. Possible end
        printf("MUTEX LOG: unlock(&chat->mutex)\n");
        fflush(stdout);

        return create_error_json("You have to rights\n");
    }

    printf("MUTEX LOG: lock(call_data->general_data->clients_mutex)\n");
    fflush(stdout);
    // Critical resource access: CLIENTS HASH TABLE. Start
    pthread_mutex_lock(call_data->general_data->clients_mutex);
    client_t *contact_data = ht_get(call_data->general_data->clients, user_to_delete_id);
    pthread_mutex_unlock(call_data->general_data->clients_mutex);
    // Critical resource access: CLIENTS HASH TABLE. End
    printf("MUTEX LOG: unlock(call_data->general_data->clients_mutex)\n");
    fflush(stdout);

    if (contact_data == NULL) {
        pthread_mutex_unlock(&chat->mutex);
        // Critical resource access: SELECTED CHAT. Possible end
        printf("MUTEX LOG: unlock(&chat->mutex)\n");
        fflush(stdout);

        return create_error_response_json("No such user exists\n", user_to_delete_id, chat_id);
    }

    if (!group_has_such_user(chat, user_to_delete_id)) {
        pthread_mutex_unlock(&chat->mutex);
        // Critical resource access: SELECTED CHAT. Possible end
        printf("MUTEX LOG: unlock(&chat->mutex)\n");
        fflush(stdout);

        return create_error_response_json("No such group user\n", user_to_delete_id, chat_id);
    }

    printf("MUTEX LOG: lock(call_data->general_data->db_mutex)\n");
    fflush(stdout);
    // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
    int rc = delete_usr_from_group(call_data->general_data->db, user_to_delete_id, chat_id);
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End
    printf("MUTEX LOG: unlock(call_data->general_data->db_mutex)\n");
    fflush(stdout);

    if (rc != 1) {
        pthread_mutex_unlock(&chat->mutex);
        // Critical resource access: SELECTED CHAT. Possible end
        printf("MUTEX LOG: unlock(&chat->mutex)\n");
        fflush(stdout);

        return create_error_response_json("Something went wrong\n", user_to_delete_id, chat_id);
    }

    remove_from_group_users(chat, user_to_delete_id);

    cJSON *notification_json_to_user = notif_you_were_deleted_from_group(chat_id, chat->name);

    printf("MUTEX LOG: lock(&contact_data->user_data->mutex)\n");
    fflush(stdout);
    // Critical resource access: CLIENT USER DATA. Start
    pthread_mutex_lock(&contact_data->user_data->mutex);
    cJSON *notification_json_to_group = notif_user_was_deleted_from_group(
        chat_id, chat->name,
        user_to_delete_id, contact_data->user_data->nickname
    );
    pthread_mutex_unlock(&contact_data->user_data->mutex);
    // Critical resource access: CLIENT USER DATA. End
    printf("MUTEX LOG: unlock(&contact_data->user_data->mutex)\n");
    fflush(stdout);

    send_to_group_and_delete_json(call_data, &notification_json_to_group, chat);

    pthread_mutex_unlock(&chat->mutex);
    // Critical resource access: SELECTED CHAT. End
    printf("MUTEX LOG: unlock(&chat->mutex)\n");
    fflush(stdout);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);

    printf("MUTEX LOG: lock(&contact_data->user_data->mutex)\n");
    fflush(stdout);
    // Critical resource access: CLIENT USER DATA. Start
    pthread_mutex_lock(&contact_data->user_data->mutex);
    remove_from_users_groups(contact_data->user_data, chat_id);
    cJSON_AddStringToObject(response_json, "nickname", contact_data->user_data->nickname);
    pthread_mutex_unlock(&contact_data->user_data->mutex);
    // Critical resource access: CLIENT USER DATA. End
    printf("MUTEX LOG: unlock(&contact_data->user_data->mutex)\n");
    fflush(stdout);

    send_to_client_and_delete_json(&notification_json_to_user, contact_data);
    
    

    
    cJSON_AddNumberToObject(response_json, "user_id", user_to_delete_id);
    cJSON_AddNumberToObject(response_json, "chat_id", chat_id);

    return response_json;
}

