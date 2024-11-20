#include "create_json.h"

static cJSON* create_chat_user_json(chat_user chat_user) {
    cJSON *chat_user_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(chat_user_json, "id", chat_user.id);
    cJSON_AddBoolToObject(chat_user_json, "active", chat_user.active);
    cJSON_AddNumberToObject(chat_user_json, "unread_messages", chat_user.unread_mes_qty);
    cJSON_AddStringToObject(chat_user_json, "nickname", chat_user.nickname);

    return chat_user_json;
}

cJSON* handle_get_my_clients_userslist(call_data_t *call_data) {
    int chat_users_num = 0;
    chat_user* chat_users =  get_clients_userslist(
        call_data->general_data->db,
        call_data->client_data->user_data->user_id,
        false, &chat_users_num);

    if (chat_users == NULL) {
        return create_error_json("No data was found\n");
    }

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "number_of_users", chat_users_num);
    cJSON *users_array = cJSON_AddArrayToObject(response_json, "users");

    for (int i = 0; i < chat_users_num; i++) {
        cJSON *chat_user_json = create_chat_user_json(chat_users[i]);
        client_t *chat_user_data = ht_get(call_data->general_data->clients, chat_users[i].id);
        cJSON_AddBoolToObject(chat_user_json, "online", chat_user_data->user_data->is_online);
        cJSON_AddItemToArray(users_array, chat_user_json);
    }
    free(chat_users);

    return response_json;
}
