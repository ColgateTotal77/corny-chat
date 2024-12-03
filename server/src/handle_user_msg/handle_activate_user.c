#include "create_json.h"
#include "sql.h"
#include "command_codes.h"


static cJSON *user_activated_notification(int user_id, char *user_login, char *user_nickname) {
    cJSON *notification_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(notification_json, "event_code", USER_WAS_ACTIVATED);
    cJSON_AddNumberToObject(notification_json, "user_id", user_id);
    cJSON_AddStringToObject(notification_json, "login", user_login);
    cJSON_AddStringToObject(notification_json, "nickname", user_nickname);

    return notification_json;
}


cJSON *handle_activate_user(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "user_login")) {
        return create_error_json("Invalid json format\n");
    }

    if (!call_data->client_data->user_data->is_admin) {
        return create_error_json("You have to rights\n");
    }

    cJSON *user_login_json = cJSON_GetObjectItemCaseSensitive(json, "user_login");
    int user_to_activate = ht_str_get(call_data->general_data->login_to_id, user_login_json->valuestring);

    if (user_to_activate == -1 ||
        user_to_activate == 1) {
        return create_error_json("No such user\n");
    }
    
    if (user_to_activate == call_data->client_data->user_data->user_id) {
        return create_error_json("You can't activate your self. Use another admin account\n");
    }

    client_t *activated_user_data = ht_get(call_data->general_data->clients,
                                           user_to_activate);
    bool is_active = true;

    if (activated_user_data == NULL) {
        cJSON *error_response = create_error_json("No such user\n");
        cJSON_AddNumberToObject(error_response, "user_id", user_to_activate);
        cJSON_AddStringToObject(error_response, "login", user_login_json->valuestring);
        return error_response;
    }

    if (activated_user_data->user_data->is_active == is_active) {
        cJSON *error_response = create_error_json("Already activated\n");
        cJSON_AddNumberToObject(error_response, "user_id", user_to_activate);
        cJSON_AddStringToObject(error_response, "login", user_login_json->valuestring);
        return error_response;
    }

    
    activate_deactivate_user(
        call_data->general_data->db,
        user_to_activate, is_active);


    activated_user_data->user_data->is_active = true;

    cJSON *user_activated_notif = user_activated_notification(
        activated_user_data->user_data->user_id,
        activated_user_data->user_data->login,
        activated_user_data->user_data->nickname
    );
    send_to_another_ids_and_delete_json(call_data, &user_activated_notif);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "user_id", user_to_activate);
    cJSON_AddStringToObject(response_json, "login", user_login_json->valuestring);


    return response_json;
}
