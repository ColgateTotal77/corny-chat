#include "cJSON.h"
#include "commands.h"
#include "client.h"

void update_my_nickname(SSL *ssl, char *new_nickname) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", UPDATE_NICKNAME);
    cJSON_AddStringToObject(json, "new_nickname", new_nickname);

    send_and_delete_json(ssl, &json);
}
