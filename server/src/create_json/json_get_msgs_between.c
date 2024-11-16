#include "create_json.h"


cJSON* json_get_new_msgs_between(call_data_t *call_data, int collocutor_id) {
    int user_id = call_data->client_data->user_data->user_id;
    int msgs_num = 0;
    client_t *sender_data = ht_get(call_data->general_data->clients, collocutor_id);
    if (sender_data == NULL) {
        return NULL;
    }
    s_message* new_messages = get_new_mess_between(call_data->general_data->db,
                                                   user_id,
                                                   collocutor_id, &msgs_num);

    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "sender_id",  collocutor_id);
    cJSON_AddStringToObject(json, "sender_login", sender_data->user_data->login);
    cJSON_AddNumberToObject(json, "msgs_num",  msgs_num);
    
    add_db_messages_to_json(new_messages, msgs_num, &json, "new_messages");

    free(new_messages);

    return json;
}
