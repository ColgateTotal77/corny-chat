#include "server.h"
#include "stdbool.h"
#include "cJSON.h"
#include "commands.h"
#include "handlers.h"
#include "sql.h"

static void handle_user_valid_msg(char *client_msg, int *leave_flag, call_data_t *call_data) {
    printf("%s\n", client_msg);
    cJSON *msg_json = cJSON_Parse(client_msg);
    cJSON *command_json = cJSON_GetObjectItemCaseSensitive(msg_json, "command_code");

    int command = (int)cJSON_GetNumberValue(command_json);

    switch (command) {
    case SEND_TO_CHAT:
        handle_send_to_chat(call_data, msg_json);
        break;
    case SEND_TO_USER:
        handle_send_to_user(call_data, msg_json);
        break;
    case CREATE_CHAT:
        handle_create_chat(call_data, msg_json);
        break;
    case SEE_ALL_CHATS:
        handle_see_all_chats(call_data);
        break;
    case SEE_ALL_USERS:
        handle_see_all_users(call_data);
        break;
    case ADD_CONTACT:
        handle_add_contact(call_data, msg_json);
        break;
    case SEE_MY_CONTACTS:
        handle_see_my_contacts(call_data);
        break;
    case JOIN_CHAT:
        handle_join_chat(call_data, msg_json);
        break;
    case ADD_CONTACT_TO_CHAT:
        handle_add_contact_to_chat(call_data, msg_json);
        break;
    case EXIT:
        send_user_exit_msg(call_data);
        *leave_flag = 1;
        break;
    default:
        printf("Wrond command code was given!!!");
        fflush(stdout);
        break;
    }

    //if (cJSON_HasObjectItem(msg_json, "message")) {
    //    cJSON *message_json = cJSON_GetObjectItemCaseSensitive(msg_json, "message");
//
    //    pthread_mutex_lock(call_data->general_data->db_mutex);
//
    //    //sql_insert_msg(call_data->db, call_data->client_data->user_data->user_id, message_json->valuestring);
    //    pthread_mutex_unlock(call_data->general_data->db_mutex);
    //}

    


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


