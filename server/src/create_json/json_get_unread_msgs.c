#include "server.h"
#include "sql.h"


cJSON *json_get_unread_msgs_counts(call_data_t *call_data) {
    int user_id = call_data->client_data->user_data->user_id;
    int senders_num = 0;

    s_unread* unread_msgs_counts = get_senders_list(call_data->general_data->db, user_id, &senders_num);

    cJSON *json = cJSON_CreateObject();
    cJSON *unread_msgs_info = cJSON_AddArrayToObject(json, "unread_messages_counts");

    for (int i = 0; i < senders_num; i++) {
        s_unread msg_count = unread_msgs_counts[i];
        client_t *sender_data = ht_get(call_data->general_data->clients, msg_count.sender_id);

        cJSON *msg_count_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(msg_count_json, "sender_id", msg_count.sender_id);
        cJSON_AddStringToObject(msg_count_json, "sender_login", sender_data->user_data->login);
        cJSON_AddNumberToObject(msg_count_json, "unread_msgs_qty", msg_count.unread_mes_qty);

        cJSON_AddItemToArray(unread_msgs_info, msg_count_json);
    }
    free(unread_msgs_counts);

    return json;
}
