#include "cJSON.h"
#include "commands.h"
#include "client.h"

void get_num_of_msgs_from_group(SSL *ssl, int group_id, int before, int quantity) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", GET_NUM_OF_MSGS_FROM_GROUP); 
    cJSON_AddNumberToObject(json, "group_id", group_id);
    cJSON_AddNumberToObject(json, "before", before);
    cJSON_AddNumberToObject(json, "quantity", quantity);

    send_and_delete_json(ssl, &json);
}
