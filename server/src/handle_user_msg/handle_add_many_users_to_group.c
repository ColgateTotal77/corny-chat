#include "server.h"
#include "cJSON.h"
#include "../libmx/inc/libmx.h"
#include "create_json.h"


cJSON *handle_add_many_users_to_group(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "users_id")
        || !cJSON_HasObjectItem(json, "chat_id")) {
        printf("invalid format\n");
        return create_error_json("Invalid json format\n");
    }

    cJSON *users_id_json = cJSON_GetObjectItemCaseSensitive(json, "users_id");
    if (!cJSON_IsArray(users_id_json)) {
        printf("not array\n");
        return create_error_json("Invalid json format\n");
    }

    cJSON *chat_id_json = cJSON_GetObjectItemCaseSensitive(json, "chat_id");
    int chat_id = (int)cJSON_GetNumberValue(chat_id_json);

    chat_t *chat = ht_get(call_data->general_data->chats, chat_id);
    if (chat == NULL) {
        return create_error_json("No such chat\n");
    }

    if (!is_user_group_owner(call_data, chat_id)) {
        return create_error_json("You have to rights\n");
    }

    int users_count = cJSON_GetArraySize(users_id_json);
    int *users_to_add = malloc(0);
    int users_num = 0;
    
    for (int i = 0; i < users_count; i++) {
        cJSON *user_id_json = cJSON_GetArrayItem(users_id_json, i);
        int user_id = (int)cJSON_GetNumberValue(user_id_json);
        if (num_inarray(users_to_add, users_num, user_id)
            || group_has_such_user(chat, user_id)
            || !user_exists(call_data, user_id)) {
            free(users_to_add);
            printf("invalid data%d %d %d\n", 
                num_inarray(users_to_add, users_num, user_id),
                group_has_such_user(chat, user_id),
                !user_exists(call_data, user_id));
            return create_error_json("Invalid json format\n");
        }
        append_to_intarr(&users_to_add, &users_num, user_id);
    }

    add_users_to_group(call_data->general_data->db, chat_id, users_to_add, users_num);

    for (int i = 0; i < users_count; i++) {
        client_t *client_data = ht_get(call_data->general_data->clients, users_to_add[i]);
        update_group_users_and_user_groups(chat, client_data);
        cJSON *notification_json = you_were_added_to_group_notification( 
            call_data->client_data->user_data->user_id,
            call_data->client_data->user_data->nickname,
            chat_id,
            chat->name);
        send_to_id_and_delete_json(call_data, &notification_json, users_to_add[i]);
    }

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON *users_id_array = cJSON_CreateIntArray(users_to_add, users_count);
    cJSON_AddItemToObject(response_json, "users_id", users_id_array);
    cJSON_AddNumberToObject(response_json, "chat_id", chat_id);

    free(users_to_add);

    return response_json;
}

