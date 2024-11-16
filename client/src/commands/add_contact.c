#include "cJSON.h"
#include "commands.h"
#include "client.h"

void add_contact(SSL *ssl, int new_contact_id) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", ADD_CONTACT); 
    cJSON_AddNumberToObject(json, "new_contact_id", new_contact_id);

    send_and_delete_json(ssl, &json);
}
