//
// Created by konst on 23.11.24.
//

#include "../../inc/sql.h"

/** function frees allocated memory.
 *  Use it to free array that is obtained in get_group_occupants_list func.
 *
 * @param groups
 * @param groups_qty
 */
void free_groups_full_list(s_group* groups, const int groups_qty) {
    for (int i = 0; i < groups_qty; i++) {
        free(groups[i].occupants);
    }
    free(groups);
    groups = NULL;
}
