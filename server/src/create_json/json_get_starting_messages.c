#include "create_json.h"


cJSON* json_get_starting_messages(call_data_t *call_data) {
    int user_id = call_data->client_data->user_data->user_id;
    int textings_num = 0;
    s_texting* textings = get_starting_messages(call_data->general_data->db, user_id, &textings_num);

    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "unreaded_chats_qty",  textings_num);
    cJSON *new_messages_json = cJSON_AddArrayToObject(json, "unread_chats_data");

    for (int i = 0; i < textings_num; i++) {
        s_texting texting = textings[i];

        cJSON *message_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(message_json, "user_id", texting.user1_id);
        cJSON_AddNumberToObject(message_json, "sender", texting.user2_id);
        cJSON_AddNumberToObject(message_json, "unread_msgs_qty", texting.unread_mes_qty);
        cJSON_AddNumberToObject(message_json, "all_msgs_qty", texting.all_mes_qty);
        add_db_messages_to_json(texting.messages, texting.unread_mes_qty,
                                &message_json, "last_messages");

        cJSON_AddItemToArray(new_messages_json, message_json);
    }

    free_texting(textings, textings_num);

    return json;
}
