#include "sending_functions.h"
#include "sql.h"
#include "command_codes.h"

static cJSON *nickname_changed_notification(int user_id, char *nickname) {
    cJSON *notification_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(notification_json, "event_code", NICKNAME_CHANGED);
    cJSON_AddNumberToObject(notification_json, "user_id", user_id);
    cJSON_AddStringToObject(notification_json, "nickname", nickname);

    return notification_json;
}

cJSON *handle_update_nickname(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "new_nickname")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *new_nickname_json = cJSON_GetObjectItemCaseSensitive(json, "new_nickname");
    int user_id = call_data->client_data->user_data->user_id;

    // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
    int return_code = update_nickname(call_data->general_data->db, user_id,
                                   new_nickname_json->valuestring);
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End

    bool success = (return_code == SQLITE_DONE);

    if (!success) {
        return create_error_json("Something went wrong\n");
    }

    // Critical resource access: USER DATA. Start
    pthread_mutex_lock(&call_data->client_data->user_data->mutex);
    bzero(call_data->client_data->user_data->nickname,
         sizeof(call_data->client_data->user_data->nickname));
    strcpy(call_data->client_data->user_data->nickname,
           new_nickname_json->valuestring);
    pthread_mutex_unlock(&call_data->client_data->user_data->mutex);
    // Critical resource access: USER DATA. End

    cJSON *nickname_changed_notif = nickname_changed_notification(
        user_id, 
        new_nickname_json->valuestring
    );
    send_to_another_ids_and_delete_json(call_data, &nickname_changed_notif);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddStringToObject(response_json, "new_nickname", new_nickname_json->valuestring);

    return response_json;
}
