#include "server.h"
#include "stdbool.h"
#include "cJSON.h"
#include "commands.h"
#include "handlers.h"
#include "sql.h"
#include "create_json.h"

static void handle_user_valid_msg(char *client_msg, int *leave_flag, call_data_t *call_data) {
    printf("%s\n", client_msg);
    cJSON *msg_json = cJSON_Parse(client_msg);
    cJSON *command_json = cJSON_GetObjectItemCaseSensitive(msg_json, "command_code");

    int command = (int)cJSON_GetNumberValue(command_json);
    cJSON *response_json = NULL;

    switch (command) {
    case SEND_TO_CHAT:
        response_json = handle_send_to_chat(call_data, msg_json);
        break;
    case SEND_TO_USER:
        response_json = handle_send_to_user(call_data, msg_json);
        break;
    case CREATE_CHAT:
        response_json = handle_create_chat(call_data, msg_json);
        break;
    case SEE_ALL_CHATS:
        response_json = handle_see_all_chats(call_data);
        break;
    case SEE_ALL_USERS:
        response_json = handle_see_all_users(call_data);
        break;
    case ADD_CONTACT:
        response_json = handle_add_contact(call_data, msg_json);
        break;
    case SEE_MY_CONTACTS:
        response_json = handle_see_my_contacts(call_data);
        break;
    case JOIN_CHAT:
        response_json = handle_join_chat(call_data, msg_json);
        break;
    case ADD_CONTACT_TO_CHAT:
        response_json = handle_add_contact_to_chat(call_data, msg_json);
        break;
    case EXIT:
        send_user_exit_msg(call_data);
        *leave_flag = 1;
        break;
    case GET_MY_CONTACTS:
        response_json = handle_get_my_contacts(call_data);
        break;
    case GET_ALL_TALKS:
        response_json = handle_get_all_talks(call_data);
        break;
    default:
        response_json = create_error_json("Wrond command code was given!!!");
        command = ERROR;
        printf("Wrond command code was given!!!");
        fflush(stdout);
        break;
    }

    if (!(*leave_flag)) {
        cJSON_AddNumberToObject(response_json, "command_code", command);
        send_to_user_and_delete_json(call_data, &response_json);
    }

    cJSON_Delete(msg_json);
}


void handle_user_msg(int bytes_received, int *leave_flag, char *client_msg, call_data_t *call_data) {
    if (bytes_received > 0) {
		if (strlen(client_msg) <= 0) { return; }
        
		handle_user_valid_msg(client_msg, leave_flag, call_data);
	}
    else if (bytes_received == 0) {
        send_user_exit_msg(call_data);
		*leave_flag = 1;
	}
    else {
		printf("ERROR: -1\n");
		*leave_flag = 1;
	}
}


