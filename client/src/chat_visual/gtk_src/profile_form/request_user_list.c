#include "GTK.h"

void request_user_list(SSL *ssl) {
    cJSON *command = cJSON_CreateObject();
    if (!command) {
        g_print("Error: Failed to create JSON object\n");
        return;
    }
    
    cJSON_AddNumberToObject(command, "command_code", 17);
    send_and_delete_json(ssl, &command);
}
