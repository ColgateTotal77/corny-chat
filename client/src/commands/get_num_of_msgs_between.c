#include "cJSON.h"
#include "commands.h"
#include "client.h"

void get_num_of_msgs_with_user(SSL *ssl, int user2, int before, int quantity) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", GET_NUM_OF_MSGS_BETWEEN); 
    cJSON_AddNumberToObject(json, "user2", user2);
    cJSON_AddNumberToObject(json, "before", before);
    cJSON_AddNumberToObject(json, "quantity", quantity);

    send_and_delete_json(ssl, &json);
}
