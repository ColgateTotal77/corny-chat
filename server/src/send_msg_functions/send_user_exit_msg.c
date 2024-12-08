#include "sending_functions.h"
#include "command_codes.h"


void send_user_exit_msg(call_data_t *call_data) {
	cJSON *json = cJSON_CreateObject();
	cJSON_AddNumberToObject(json, "event_code", USER_LEFT);
	cJSON_AddNumberToObject(json, "user_id", call_data->client_data->user_data->user_id);

	send_to_another_ids_and_delete_json(call_data, &json);
}

