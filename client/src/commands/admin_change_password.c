#include "cJSON.h"
#include "commands.h"
#include "client.h"

void admin_change_password(SSL *ssl, int user_id, char *new_password) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", ADMIN_CHANGE_PASSWORD);
    cJSON_AddNumberToObject(json, "user_id", user_id);
    cJSON_AddStringToObject(json, "new_password", new_password);

    send_and_delete_json(ssl, &json);
}
