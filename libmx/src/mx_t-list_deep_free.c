#include <stdlib.h>
#include "libmx.h"
#include "tlist.h"

/**
 * @brief frees t_list and all data
 *
 * @param list
 */
void mx_t_list_deep_free(t_list* list) {
	t_list* current = list;
	while (current) {
		free(current->data);
		current = current->next;
	}
	mx_free_node_list(&list);
}
