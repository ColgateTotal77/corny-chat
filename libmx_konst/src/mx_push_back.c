#include <stddef.h>
#include "tlist.h"
t_list* mx_create_node(void* data);


void mx_push_back(t_list** list, void* data) {
	t_list* new_node = mx_create_node(data);

	if (!new_node)
		return;
	if (list == NULL || *list == NULL) {
		*list = new_node;
		return;
	}
	t_list* my_list = *list;

	while (my_list->next)
		my_list = my_list->next;
	my_list->next = new_node;
}
