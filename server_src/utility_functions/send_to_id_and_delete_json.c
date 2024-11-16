#include "server.h"


void send_to_id_and_delete_json(call_data_t *call_data, cJSON **json, int id_to_send) {
    char *str_json = cJSON_Print(*json);
    cJSON_Minify(str_json);

    send_message_to_id(call_data, str_json, id_to_send);

    free(str_json);
    cJSON_Delete(*json);
}
