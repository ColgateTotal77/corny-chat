#include "server.h"
#include "password.h"
#include "cJSON.h"
#include "sql.h"


static int create_new_user_and_return_id(call_data_t *call_data, char *login, unsigned char* password_hash) {
    user_create *usr = (user_create*)malloc(sizeof(user_create));
	init_user_create(usr, login, login, password_hash, 0);
    int user_id = create_user(call_data->general_data->db, *usr);

    free(usr);

    return user_id;
}


enum LoginValidationResult find_or_create_user(call_data_t *call_data, 
                                               cJSON *json_name_password, int *user_id) {
    cJSON *name = cJSON_GetObjectItemCaseSensitive(json_name_password, "name");
	cJSON *password = cJSON_GetObjectItemCaseSensitive(json_name_password, "password");
    enum LoginValidationResult validation_result;

    if (!check_login(name->valuestring) || !check_password(password->valuestring)) { 
        return INVALID_INPUT;
	}

    unsigned char *db_hash = get_password_hash(call_data->general_data->db, name->valuestring);
    unsigned char *input_hash = hash_password(password->valuestring, name->valuestring);


    if (db_hash == NULL) {
        *(user_id) = create_new_user_and_return_id(call_data, name->valuestring, input_hash);
        free(input_hash);
        
        return NO_SUCH_USER;
    }

    if (memcmp(db_hash, input_hash, 32) == 0) {
        validation_result = VALID_LOGIN;
    }
    else {
        validation_result = INVALID_INPUT;
    }

    free(db_hash);
    free(input_hash);

    return validation_result;
}

