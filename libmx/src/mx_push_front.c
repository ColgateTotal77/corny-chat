#include <stdlib.h>

#include "tlist.h"
t_list* mx_create_node(void* data);


void mx_push_front(t_list** list, void* data) {
	t_list* new_node = mx_create_node(data);

	if (!new_node)
		return;
	if (list == NULL || *list == NULL) {
		*list = new_node;
		return;
	}
	new_node->next = *list;
	*list = new_node;
}
