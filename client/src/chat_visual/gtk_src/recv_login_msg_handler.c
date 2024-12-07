#include "GTK.h"


void* recv_login_msg_handler(void* arg) {
    GTK_data_t *GTK_data = (GTK_data_t*)arg;
    call_data_t *call_data = GTK_data->call_data;
    cJSON *parsed_json;

    GTK_data->stop_login = malloc(sizeof(bool));
    *(GTK_data->stop_login) = false;

    while (!(*(GTK_data->stop_login))) {
        char* message = NULL;
        int bytes_received = recieve_next_response(call_data->ssl, &message);

        if (bytes_received > 0) {
            printf("\nLOGIN message = %s\n",message);
            parsed_json = cJSON_Parse(message);
            if (!parsed_json) {
                continue;
            }
            cJSON *command_code_json = cJSON_GetObjectItemCaseSensitive(parsed_json, "command_code");
            if (command_code_json) {
                if ((command_code_json)->valueint == 11) {
                    pthread_mutex_lock(&GTK_data->pthread_mutex);
                    if(cJSON_GetObjectItemCaseSensitive(parsed_json, "success")->valueint > 0) {
                        cJSON *session_id_json = cJSON_GetObjectItemCaseSensitive(parsed_json, "session_id");
                        GTK_data->session_id = (char*)calloc(strlen(session_id_json->valuestring)+ 1, sizeof(char));
                        strncpy(GTK_data->session_id, session_id_json->valuestring, strlen(session_id_json->valuestring));
                        GTK_data->user_id = cJSON_GetObjectItemCaseSensitive(parsed_json, "user_id")->valueint;
                        char* nickname = cJSON_GetObjectItemCaseSensitive(parsed_json, "nickname")->valuestring;
                        GTK_data->username = (char*)calloc(strlen(nickname)+ 1, sizeof(char));
                        strncpy(GTK_data->username, nickname, strlen(nickname));

                        GTK_data->is_admin = cJSON_GetObjectItemCaseSensitive(parsed_json, "is_admin")->valueint;

                        GTK_data->login_successful = true;
                        *(GTK_data->stop_login) = true;
                    }else{
                        call_data->ssl = setup_new_connection(call_data->host, call_data->port);
                    }
                    
                    pthread_cond_signal(&GTK_data->pthread_cond);
                    pthread_mutex_unlock(&GTK_data->pthread_mutex);
                    cJSON_Delete(parsed_json);
                    free(message);
                    continue;
                }
            }
            cJSON_Delete(parsed_json);
        }
        free(message);
    }
    pthread_detach(pthread_self());
    return NULL;
}
