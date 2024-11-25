//
// Created by konst on 23.11.24.
//

#include "../../libmx/inc/libmx.h"

#include "../../inc/sql.h"


char *six_var_strbuilder(char *s1, char *s2, char *s3, char *s4, char *s5, char *s6);


/**
 * @brief returns an array of groups that the user is included in. If user_id = 0 returns all groups;
 * @warning function allocates memory. Use free_groups_full_list function to free.
 * @note if user_id == 0 unread_mes_qty == -1
 *
 * @param db
 * @param user_id id of the user who is a groups member
 * @param group_qty pointer to int variable that will save group quantity
 * @return array[s_group structures]
 */
s_group *get_groups_full_list(sqlite3 *db, const int user_id, int *group_qty) {
    char *sql = NULL;
    char *sql_first_line_user = "SELECT g.id, g.name, g.createdAt, g.ownerId, gu.qty, gu2.unreadNum ";
    char *sql_first_line_no_user = "SELECT g.id, g.name, g.createdAt, g.ownerId, gu.qty ";
    char *sql_basic = "FROM groups g "
            "JOIN "
            "(SELECT groupId , COUNT(*) AS qty "
            "FROM group_users "
            "GROUP BY groupId) AS gu ON g.id = gu.groupId ";
    sqlite3_stmt *stmt;
    if (user_id > 0) {
        char *sql_addition = "JOIN group_users gu2 ON g.id = gu2.groupId "
                "WHERE gu2.userId = ? AND g.id IN"
                "(SELECT groupId "
                "FROM group_users gu "
                "WHERE userId = ?)";
        sql = six_var_strbuilder(sql_first_line_user, sql_basic, sql_addition, "", "", "");
    } else {
        sql = six_var_strbuilder(sql_first_line_no_user, sql_basic, ";", "", "", "");
    }
    if (!sql) {
        printf("Error allocation memory\n");
        return NULL;
    }
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        free(sql);
        return NULL;
    }
    free(sql);
    if (user_id > 0) {
        sqlite3_bind_int(stmt, 1, user_id);
        sqlite3_bind_int(stmt, 2, user_id);
    }

    t_list *list = NULL;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        s_group *group = malloc(sizeof(s_group));
        init_s_group(group,
                     sqlite3_column_int(stmt, 0),
                     (char *) sqlite3_column_text(stmt, 1),
                     (char *) sqlite3_column_text(stmt, 2),
                     sqlite3_column_int(stmt, 3),
                     sqlite3_column_int(stmt, 4),
                     NULL);
        int *occ = get_group_occupants_list(db, group->id, group->occupants_num);
        group->occupants = occ;
        group->unread_mes_qty = user_id ? sqlite3_column_int(stmt, 5) : -1;
        mx_push_back(&list, group);
    }
    *group_qty = mx_list_size(list);
    s_group *groups = malloc(sizeof(s_group) * (*group_qty));

    if (!groups) {
        mx_t_list_deep_free(list);
        sqlite3_finalize(stmt);
        return NULL;
    }
    if (*group_qty > 0) {
        t_list *temp_list = list;
        int i = 0;
        while (temp_list) {
            s_group *group = temp_list->data;
            groups[i] = *group;
            temp_list = temp_list->next;
            i++;
        }
    }
    mx_t_list_deep_free(list);
    sqlite3_finalize(stmt);
    return groups;
}
