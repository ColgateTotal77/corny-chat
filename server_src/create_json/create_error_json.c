#include "create_json.h"

cJSON *create_error_json(char *err_msg) {
    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(response_json, "success", false);
    cJSON_AddStringToObject(response_json, "err_msg", err_msg);

    return response_json;
}
