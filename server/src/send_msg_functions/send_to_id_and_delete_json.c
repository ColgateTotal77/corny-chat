#include "server.h"


void send_to_id_and_delete_json(call_data_t *call_data, cJSON **json, int id_to_send) {
    char *str_json = cJSON_Print(*json);
    cJSON_Minify(str_json);

    char *response = add_length_to_string(str_json);
    free(str_json);
    
    send_message_to_id(call_data, response, id_to_send);

    free(response);
    cJSON_Delete(*json);
}
