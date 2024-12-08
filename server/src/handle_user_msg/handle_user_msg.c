#include "sending_functions.h"
#include "stdbool.h"
#include "cJSON.h"
#include "handlers.h"
#include "sql.h"
#include "create_json.h"
#include "command_codes.h"

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
    case UPDATE_NICKNAME:
        response_json = handle_update_nickname(call_data, msg_json);
        break;
    case CHANGE_PASSWORD:
        response_json = handle_change_password(call_data, msg_json);
        break;
    case CREATE_USER:
        response_json = handle_create_user(call_data, msg_json);
        break;
    case GET_ALL_CLIENTS_USERSLIST:
        response_json = handle_get_all_clients_userslist(call_data);
        break;
    case MARK_CHAT_MSGS_AS_READED:
        response_json = handle_mark_chat_msgs_as_readed(call_data, msg_json);
        break;
    case GET_MY_CLIENTS_USERSLIST:
        response_json = handle_get_my_clients_userslist(call_data);
        break;
    case DELETE_USER_FROM_GROUP:
        response_json = handle_delete_user_from_group(call_data, msg_json);
        break;
    case ADD_MANY_USERS_TO_GROUP:
        response_json = handle_add_many_users_to_group(call_data, msg_json);
        break;
    case DELETE_GROUP:
        response_json = handle_delete_group(call_data, msg_json);
        break;
    case ADMIN_CHANGE_PASSWORD:
        response_json = handle_admin_change_password(call_data, msg_json);
        break;
    case GET_NUM_OF_MSGS_BETWEEN:
        response_json = handle_get_last_msgs_between(call_data, msg_json);
        break;
    case DEACTIVATE_USER:
        response_json = handle_deactivate_user(call_data, msg_json);
        break;
    case ACTIVATE_USER:
        response_json = handle_activate_user(call_data, msg_json);
        break;
    case GET_NUM_OF_MSGS_FROM_GROUP:
        response_json = handle_get_num_of_msgs_from_group(call_data, msg_json);
        break;
    case UPDATE_MESSAGE:
        response_json = handle_update_message(call_data, msg_json);
        break;
    case DELETE_MESSAGE:
        response_json = handle_delete_message(call_data, msg_json);
        break;
    case GET_MY_GROUPS:
        response_json = handle_get_my_groups(call_data);
        break;
    case MARK_GROUP_MSGS_AS_READED:
        response_json = handle_mark_group_msgs_as_readed(call_data, msg_json);
        break;
    case GET_CHAT_USERS:
        response_json = handle_get_chat_users(call_data, msg_json);
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


void handle_user_msg(int bytes_received, int *leave_flag, 
                     char *client_msg, call_data_t *call_data) {
    if (bytes_received > 0) {
		if (strlen(client_msg) <= 0) { return; }

        if (!call_data->client_data->user_data->is_active) {
            send_user_exit_msg(call_data);
            *leave_flag = 1;
            return;
        }
        
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


