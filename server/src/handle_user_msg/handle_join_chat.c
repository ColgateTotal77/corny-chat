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
    chat_t *chat = ht_get(call_data->general_data->chats, chat_id);

    if (chat == NULL) {
        cJSON *error_response = create_error_json("No such chat\n");
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    if (group_has_such_user(chat, user_id)) {
        cJSON *error_response = create_error_json("Already joined\n");
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    const int users[] = {user_id};
	int res = add_users_to_group(call_data->general_data->db, chat_id, users, 1);
    if (res <= 0) {
        cJSON *error_response = create_error_json("Something went wrong\n");
        cJSON_AddNumberToObject(error_response, "chat_id", chat_id);
        return error_response;
    }

    append_to_group_users(chat, user_id);
    append_to_users_groups(call_data->client_data->user_data, chat_id);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "chat_joined", chat_id);
    cJSON_AddStringToObject(response_json, "chat_name", chat->name);

    return response_json;
}

