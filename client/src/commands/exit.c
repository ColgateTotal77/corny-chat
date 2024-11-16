#include "cJSON.h"
#include "commands.h"
#include "client.h"

void send_exit_command(SSL *ssl) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", EXIT);

    send_and_delete_json(ssl, &json);
}

