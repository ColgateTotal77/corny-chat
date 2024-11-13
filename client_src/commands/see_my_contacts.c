#include "cJSON.h"
#include "commands.h"
#include "client.h"

void see_my_contacts(SSL *ssl) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", SEE_MY_CONTACTS);

    send_and_delete_json(ssl, &json);
}


