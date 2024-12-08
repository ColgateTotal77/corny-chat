//
// Created by konst on 20.11.24.
//

#include "../../inc/sql.h"

/**@brief adds new group to the "groups" table
 *
 * @param db
 * @param owner_id
 * @param name
 * @return int - created group id or 0 if something went wrong
 */
static int create_new_group(sqlite3* db, const int owner_id, char* name) {
	char* sql = "INSERT INTO groups (ownerId, name) VALUES (?, ?);";
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	// Привязка параметров
	sqlite3_bind_int(stmt, 1, owner_id);
	sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return 0;
	}
	int group_id = (int)sqlite3_last_insert_rowid(db);
	sqlite3_finalize(stmt);
	printf("group '%s' created successfully.\n", name);
	return group_id;
}

/**
 * @brief creates new group and adds users
 * @brief users_list can be empty and list_len can be 0 then group will contain only the owner
 *
 * @warning you CAN NOT add owner id to users_list 
 * @warning or add the same user id in the users_list twice. That causes errors.
 *
 * @param db
 * @param owner_id user that creates the group
 * @param name group name
 * @param users_list pointer to int array of users being added to the group
 * @param list_len number of users in the list
 * @return int >= 0 created group id if ok
 * @return 0 if something went wrong while the group was being created
 * @return -1 if something went wrong while the owner  was being added
 * @return -2 if something went wrong while the other users were being added
 */
int create_group(sqlite3* db, const int owner_id, char* name, const int* users_list,
                 const int list_len) {
	const int group_id = create_new_group(db, owner_id, name);
	if (group_id == 0) return group_id;
	const int users[] = {owner_id};
	int res = add_users_to_group(db, group_id, users, 1);
	if (res <= 0) return -1;
	if (list_len == 0) return group_id;
	res = add_users_to_group(db, group_id, users_list, list_len);
	if (res == -1) return -2;
	return group_id;
}
