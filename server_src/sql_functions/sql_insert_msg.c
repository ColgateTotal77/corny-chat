#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>


int sql_insert_msg(sqlite3 *db, int usr_id, char* msg){

  char *err_msg = NULL;  // error massage pointer
  char sql_buf[256];  // buffer SQL-query
  snprintf(sql_buf, sizeof(sql_buf),  // prepare query
     "INSERT INTO messages (ownerId, message) VALUES ('%d','%s');", usr_id, msg);

  int rc = sqlite3_exec(db, sql_buf, 0, 0, &err_msg); // execute query

  // check is query successfil
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err_msg);  // output error message
    sqlite3_free(err_msg);  // free error message memory
    return rc;  // return error code
  }

  printf("message added successfuly\n");  // output success message

  return SQLITE_OK; //return success code

}
