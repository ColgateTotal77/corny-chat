#include "GTK.h"

user_list_t* create_user_list(void) {
    user_list_t *list = malloc(sizeof(user_list_t));
    list->capacity = 10;
    list->count = 0;
    list->users = malloc(sizeof(user_data_t*) * list->capacity);
    return list;
}

