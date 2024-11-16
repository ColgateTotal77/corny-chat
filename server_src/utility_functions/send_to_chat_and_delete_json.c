#include "server.h"


void send_to_chat_and_delete_json(call_data_t *call_data, cJSON **json, int chat_id) {
    char *str_json = cJSON_Print(*json);
    cJSON_Minify(str_json);

    send_message_to_chat(call_data, str_json, chat_id);

    free(str_json);
    cJSON_Delete(*json);
}
