#include "tlist.h"
#include <stdbool.h>

#include "libmx.h"

static void replace_first_nodes(t_list** head) {
	t_list* first = *head;
	t_list* second = first->next;

	first->next = second->next;
	second->next = first;
	*head = second;
}

static void replace_middle_nodes(t_list** head) {
	t_list* before = *head;
	t_list* current = before->next;
	t_list* after = current->next;

	before->next = after;
	current->next = after->next;
	after->next = current;
	*head = before;
}

t_list* mx_sort_list(t_list* lst, bool (*cmp)(void*, void*)) {
	if (!lst)
		return NULL;
	if (!cmp)
		return lst;
	const int size = mx_list_size(lst);
	if (size < 2)
		return lst;
	if (size == 2) {
		if (cmp(lst->data, lst->next->data)) {
			replace_first_nodes(&lst);
			return lst;
		}
		return lst;
	}
	for (int i = 1; i < size; i++) {
		t_list* current = lst;

		for (int j = 0; j < size - i; j++) {
			if (j == 0) {
				if (cmp(current->data, current->next->data)) {
					replace_first_nodes(&lst);
					current = lst;
				}
				continue;
			}
			if (cmp(current->next->data, current->next->next->data)) {
				replace_middle_nodes(&current);
			}
			current = current->next;
		}
	}
	return lst;
}
