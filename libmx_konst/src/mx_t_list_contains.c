
#include "libmx.h"
#include "tlist.h"

int mx_t_list_contains(t_list* list, char** str) {
	t_list* curr = list;
	for (int i = 0; i < mx_list_size(list); i++) {
		if (!mx_strcmp(curr->data, *str)) {
			return 1;
		}
		curr = curr->next;
	}
	return 0;
}
