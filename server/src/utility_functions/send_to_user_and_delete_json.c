#include "server.h"
#include "../../libmx/inc/libmx.h"


void send_to_user_and_delete_json(call_data_t *call_data, cJSON **json) {
    char *str_json = cJSON_Print(*json);
    cJSON_Minify(str_json);

    char *response = add_length_to_string(str_json);
    free(str_json);
    
    send_message_to_user(call_data, response);

    free(response);
    cJSON_Delete(*json);
}

