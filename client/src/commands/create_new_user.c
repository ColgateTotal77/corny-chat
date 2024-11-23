#include "cJSON.h"
#include "commands.h"
#include "client.h"

void create_new_user(SSL *ssl, char *login, char *password, bool is_admin) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", CREATE_USER);
    cJSON_AddStringToObject(json, "login", login); 
    cJSON_AddStringToObject(json, "password", password);
    cJSON_AddBoolToObject(json, "is_admin", is_admin);

    send_and_delete_json(ssl, &json);
}
