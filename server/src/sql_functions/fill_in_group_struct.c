//
// Created by konst on 22.11.24.
//

#include "../../inc/sql.h"

void init_s_group(s_group *group, const int id, char *name, char *created_at,
                  const int owner_id, int occupants_num, int *occupants) {
    group->id = id;
    snprintf(group->name, sizeof(group->name), "%s", name);
    snprintf(group->createdAt, sizeof(group->createdAt), "%s", created_at);
    group->owner_id = owner_id;
    group->occupants_num = occupants_num;
    group->occupants = occupants;
}
