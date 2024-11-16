#include "server.h"

cJSON *create_response_json(int command_code, bool success_status, char *error_msg) {
    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(response_json, "command_code", command_code);
    cJSON_AddBoolToObject(response_json, "success", success_status);
    if (error_msg != NULL) {
        cJSON_AddStringToObject(response_json, "error_msg", error_msg);
    }

    return response_json;
}

