#include "create_json.h"


static cJSON *create_group_json_from_db_data(s_group group_data) {
    cJSON *group_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(group_json, "chat_id", group_data.id);
    cJSON_AddStringToObject(group_json, "chat_name", group_data.name);
    cJSON_AddStringToObject(group_json, "createdAt", group_data.createdAt);
    cJSON_AddNumberToObject(group_json, "owner_id", group_data.owner_id);
    cJSON *users_id_array = cJSON_CreateIntArray(group_data.occupants, group_data.occupants_num);
    cJSON_AddItemToObject(group_json, "members", users_id_array);
    cJSON_AddNumberToObject(group_json, "unread_mes_qty", group_data.unread_mes_qty);

    return group_json;
}

cJSON *handle_get_my_groups(call_data_t *call_data) {
    int count = 0;

    // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
    s_group *groups = get_groups_full_list(
        call_data->general_data->db,
        call_data->client_data->user_data->user_id, &count
    );
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", true);
    cJSON_AddNumberToObject(response_json, "groups_count", count);
    cJSON *groups_json_array = cJSON_AddArrayToObject(response_json, "groups");
    
    for (int i = 0; i < count; i++) {
        cJSON *group_json = create_group_json_from_db_data(groups[i]);

        cJSON_AddItemToArray(groups_json_array, group_json);
    }

    free_groups_full_list(groups, count);

    return response_json;
}

