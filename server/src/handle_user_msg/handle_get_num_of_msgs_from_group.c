#include "create_json.h"


cJSON* handle_get_num_of_msgs_from_group(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "group_id")
        || !cJSON_HasObjectItem(json, "before")
        || !cJSON_HasObjectItem(json, "quantity")) {
        return create_error_json("Invalid json format\n");
    }

    cJSON *group_id_json = cJSON_GetObjectItemCaseSensitive(json, "group_id");
    cJSON *before_id_json = cJSON_GetObjectItemCaseSensitive(json, "before");
    cJSON *quantity_json = cJSON_GetObjectItemCaseSensitive(json, "quantity");

    if (!group_exists(call_data, group_id_json->valueint)) {
        cJSON* error_response = create_error_json("Invalid json format\n");
        cJSON_AddNumberToObject(error_response, "group_id", group_id_json->valueint);
        return error_response;
    }

    int num_of_msgs_retrieved = 0;
    s_message *group_messages = get_new_group_mess(
        call_data->general_data->db,
        group_id_json->valueint,
        quantity_json->valueint,
        &num_of_msgs_retrieved,
        before_id_json->valueint
    );

    cJSON *response_json = cJSON_CreateObject();
    //cJSON_AddNumberToObject(response_json, "unread_mes_qty",  textings->unread_mes_qty);
    cJSON_AddNumberToObject(response_json, "all_mes_qty",  num_of_msgs_retrieved);
    cJSON_AddNumberToObject(response_json, "group_id", group_id_json->valueint);
    add_db_messages_to_json(group_messages, num_of_msgs_retrieved,
                            &response_json, "messages");

    free(group_messages);

    return response_json;
}
