#include "cJSON.h"
#include "commands.h"
#include "client.h"

void add_contact_to_chat(SSL *ssl, int contact_id, int chat_id) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "command_code", ADD_CONTACT_TO_CHAT); 
    cJSON_AddNumberToObject(json, "contact_id", contact_id);
    cJSON_AddNumberToObject(json, "chat_id", chat_id);

    send_and_delete_json(ssl, &json);
}

