//
// Created by konstantin on 04.11.24.
//
//#include <stdio.h>
//#include <sqlite3.h>
#include <stdlib.h>
#include "../../inc/sql.h"

/**
 *@brief adds new user to DB from appropriate structure
 *
 * @param db opened Data Base connection
 * @param usr user_create structure from sql.h
 *
 * @return success or error code
 */
int insert_user(sqlite3 *db, user_create usr) {
    char *err_msg = NULL; // error massage pointer
    char sql_buf[256]; // buffer SQL-query
    snprintf(sql_buf, sizeof(sql_buf), // prepare query
             "INSERT INTO users (login, password, nickname, roleId) VALUES ('%s', '%s', '%s', '%d');",
             usr.login, usr.password, usr.nickname, usr.role_id);

    int rc = sqlite3_exec(db, sql_buf, 0, 0, &err_msg); // execute query

    // check is query successful
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg); // output error message
        sqlite3_free(err_msg); // free error message memory
        return rc; // return error code
    }

    printf("User '%s' created successfully.\n", usr.login); // output success message
    return SQLITE_OK; //return success code
}
