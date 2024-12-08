#include "server.h"
#include "../../libmx/inc/libmx.h"


void send_to_user_and_delete_json_no_mutexes(call_data_t *call_data, cJSON **json) {
    char *str_json = cJSON_Print(*json);
    cJSON_Minify(str_json);

    char *response = add_length_to_string(str_json);
    free(str_json);
    
    int write_len = SSL_write(call_data->client_data->ssl, response, strlen(response));
    if (write_len <= 0) { 
        fprintf(stderr, "SSL_write failed with error: %d\n", 
                SSL_get_error(call_data->client_data->ssl, write_len));
        ERR_print_errors_fp(stderr);
    }

    free(response);
    cJSON_Delete(*json);
}

