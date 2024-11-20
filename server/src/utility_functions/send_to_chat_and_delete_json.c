#include "server.h"


void send_to_chat_and_delete_json(call_data_t *call_data, cJSON **json, int chat_id) {
    char *str_json = cJSON_Print(*json);
    cJSON_Minify(str_json);

    char *response = add_length_to_string(str_json);
    free(str_json);
    
    send_message_to_chat(call_data, response, chat_id);

    free(response);
    cJSON_Delete(*json);
}
