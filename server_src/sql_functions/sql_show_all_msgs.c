#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    (void)NotUsed;
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int sql_show_all_msgs(sqlite3 *db) {
    char *err_msg = NULL;
    char *sql = "SELECT * FROM messages;";

    int rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);  // output error message
        sqlite3_free(err_msg);  // free error message memory
        return 1;  // return error code
    }

    return 0;
}

