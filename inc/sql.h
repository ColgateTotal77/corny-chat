#include <sqlite3.h>


int sql_insert_msg(sqlite3 *db,int usr_id, char* msg);
int sql_show_all_msgs(sqlite3 *db);
int sql_show_all_users(sqlite3 *db);
int sql_is_valid_user(sqlite3 *db, char *name, char *password);
int sql_insert_user(sqlite3 *db, char *name, char* password);

