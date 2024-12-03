#include "GTK.h"

// Function to create a new group and add it to the group list
void create_group(group_list_t *list, int id, char *name, int id_owner, char *last_message, char *last_time) {
    // Check if we need to increase the capacity
    if (list->count >= list->capacity) {
        list->capacity *= 2; // Double the capacity
        list->group = realloc(list->group, sizeof(group_data_t*) * list->capacity); // Reallocate memory
    }
    
    // Create a new group
    group_data_t *group = malloc(sizeof(group_data_t));
    group->id = id; // Set group ID
    group->name = mx_strdup(name); // Duplicate the group name
    group->id_owner = id_owner; // Set owner ID
    group->last_message = mx_strdup(last_message ? last_message : "NULL"); // Duplicate last message
    group->last_time = mx_strdup(last_time ? last_time : "NULL"); // Duplicate last time
    
    // Add the new group to the list
    list->group[list->count++] = group; // Increment count
}

// Function to free the allocated memory for the group list
void free_group_list(group_list_t *list) {
    for (int i = 0; i < list->count; i++) {
        free(list->group[i]->name); // Free the group name
        free(list->group[i]->last_message); // Free last message
        free(list->group[i]->last_time); // Free last time
        free(list->group[i]); // Free the group structure
    }
    free(list->group); // Free the group array
    free(list); // Free the group list structure
}
