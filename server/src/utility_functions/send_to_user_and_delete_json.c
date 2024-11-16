#include "server.h"


void send_to_user_and_delete_json(call_data_t *call_data, cJSON **json) {
    char *str_json = cJSON_Print(*json);
    cJSON_Minify(str_json);

    send_message_to_user(call_data, str_json);

    free(str_json);
    cJSON_Delete(*json);
}

