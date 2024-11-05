#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    char **name;
    char **password;
    int *search_result;
} callback_data_t;

static int callback(void *callback_data_voidPtr, int argc, char **argv, char **azColName) {
    (void)argc;
    (void)azColName;
    callback_data_t *callback_data = callback_data_voidPtr;
    if (*(callback_data->search_result) != 0) {
        return 0;
    }

    printf("%s\n", argv[2]);
    printf("%s\n", *(callback_data->name));
    printf("%s\n", argv[3]);
    printf("%s\n", *(callback_data->password));
    fflush(stdout);

    int name_cmp = strcmp(*(callback_data->name), argv[2]);
    int password_cmp = strcmp(*(callback_data->password), argv[3]);

    if (name_cmp == 0 && password_cmp == 0) {
        *(callback_data->search_result) = 1;
    }
    else if (name_cmp == 0) {
        *(callback_data->search_result) = 2;
    }

    return 0;
}

int sql_is_valid_user(sqlite3 *db, char *name, char *password) {
    char *err_msg = NULL;

    char sql_buf[256];  // buffer SQL-query
    snprintf(sql_buf, sizeof(sql_buf), "SELECT * FROM users WHERE login LIKE '%s';", name);

    callback_data_t *callback_data = (callback_data_t*)malloc(sizeof(callback_data_t));
    
    callback_data->name = &name;
    callback_data->password = &password;
    int search_result = 0;
    callback_data->search_result = &search_result;

    printf("here\n");
    fflush(stdout);

    int rc = sqlite3_exec(db, sql_buf, callback, callback_data, &err_msg);

    free(callback_data);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);  // output error message
        sqlite3_free(err_msg);  // free error message memory
        return 1;  // return error code
    }

    return search_result;
}

