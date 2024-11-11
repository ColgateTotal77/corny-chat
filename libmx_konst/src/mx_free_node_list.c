#include "tlist.h"
#include "libmx.h"

void mx_free_node_list(t_list** head) {
	while (*head != NULL) {
		mx_pop_front(head);
	}
}
