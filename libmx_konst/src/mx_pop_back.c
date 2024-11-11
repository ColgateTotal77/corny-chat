#include <malloc.h>
#include <stdlib.h>

#include "tlist.h"

void mx_pop_back(t_list** head) {
	if (head == NULL || *head == NULL)
		return;
	t_list* temp = *head;

	if (temp->next == NULL) {
		free(temp);
		*head = NULL;
		return;
	}
	while (temp->next->next) {
		temp = temp->next;
	}
	free(temp->next);
	temp->next = NULL;
}
