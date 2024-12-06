#include "create_json.h"


cJSON* handle_get_last_msgs_between(call_data_t *call_data, cJSON *json) {
    if (!cJSON_HasObjectItem(json, "user2")
        || !cJSON_HasObjectItem(json, "before")
        || !cJSON_HasObjectItem(json, "quantity")) {
        return create_error_json("Invalid json format\n");
    }

    int user_id = call_data->client_data->user_data->user_id;
    cJSON *user2_json = cJSON_GetObjectItemCaseSensitive(json, "user2");
    cJSON *before_id_json = cJSON_GetObjectItemCaseSensitive(json, "before");
    cJSON *quantity_json = cJSON_GetObjectItemCaseSensitive(json, "quantity");

    // Critical resource access: DATABASE. Start
    pthread_mutex_lock(call_data->general_data->db_mutex);
    s_texting* textings = get_last_messages_between(
        call_data->general_data->db,
        user_id, user2_json->valueint,
        quantity_json->valueint, before_id_json->valueint
    );
    pthread_mutex_unlock(call_data->general_data->db_mutex);
    // Critical resource access: DATABASE. End

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(response_json, "unread_mes_qty",  textings->unread_mes_qty);
    cJSON_AddNumberToObject(response_json, "all_mes_qty",  textings->all_mes_qty);
    add_db_messages_to_json(textings->messages, textings->all_mes_qty,
                                &response_json, "messages");

    free_texting(textings, 1);

    return response_json;
}
