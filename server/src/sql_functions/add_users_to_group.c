//
// Created by konst on 20.11.24.
//

#include "../../libmx/inc/libmx.h"

#include "../../inc/sql.h"


/**
 * @brief joins 6 string (any of them can be empty)
 * @warning allocates memory, do not forget free
 *
 * @param s1
 * @param s2
 * @param s3
 * @param s4
 * @param s5
 * @param s6
 * @return pointer to the new allocated string
 */
char* six_var_strbuilder(char* s1, char* s2, char* s3, char* s4, char* s5, char* s6) {
	const int len = (int)(strlen(s1) + strlen(s2) + strlen(s3) 
	                    + strlen(s4) + strlen(s5) + strlen(s6));
	char* result = mx_strnew(len);
	mx_strcpy(result, s1);
	mx_strcat(result, s2);
	mx_strcat(result, s3);
	mx_strcat(result, s4);
	mx_strcat(result, s5);
	mx_strcat(result, s6);
	return result;
}

/**
 * @brief adds users to the group
 * @warning this func must be allowed only for group owner
 *
 * @param db
 * @param group_id
 * @param users_list
 * @param list_len
 * @return 1 if OK, 0 if list_len == 0, -1 if something went wrong
 */
int add_users_to_group(sqlite3* db, int group_id, const int* users_list, const int list_len) {
	char* pre_sql = "INSERT INTO group_users (groupId, userId) VALUES ";
	if (list_len == 0) return 0;
	sqlite3_stmt *stmt;

	// prepare users tuples
	char* gr_id = mx_itoa(group_id);
	char* first_usr = mx_itoa(users_list[0]);
	char* result = six_var_strbuilder(" (", gr_id, ", ", first_usr, ")", "");
	free(first_usr);
	for (int i = 1; i < list_len; i++) {
		char* usr = mx_itoa(users_list[i]);
		char* tuple = six_var_strbuilder(", (", gr_id, ", ", usr, ")", "");
		char* result1 = mx_strjoin(result, tuple);
		free(result);
		result = result1;
		free(usr);
		free(tuple);
	}
	free(gr_id);

	// Подготовка SQL-запроса
	char* sql = six_var_strbuilder(pre_sql, result, ";","","","");
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	free(result);
	free(sql);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	sqlite3_bind_text(stmt, 1, result, -1, SQLITE_STATIC);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	sqlite3_finalize(stmt);
	return 1;
}
