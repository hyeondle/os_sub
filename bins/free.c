#include "init.h"
#include <stdlib.h>
#include <stdio.h>

void free_mutex_list(t_mutex_list *mutex_list) {
	pthread_mutex_destroy(mutex_list->cpu);
	pthread_mutex_destroy(mutex_list->t);
	pthread_mutex_destroy(mutex_list->p);
	pthread_mutex_destroy(mutex_list->ready_queue);
	pthread_mutex_destroy(mutex_list->check);
	pthread_mutex_destroy(mutex_list->r_t);

	free(mutex_list->cpu);
	free(mutex_list->t);
	free(mutex_list->p);
	free(mutex_list->ready_queue);
	free(mutex_list->check);
	free(mutex_list->r_t);
}

void memory_free(t_setting *set, t_process *list) {
	t_process *temp;
	t_process *next;

	printf("free memory\n");
	free_mutex_list(set->mutex_list);
	free(set->values->ready_queue);
	free(set->values);
	free(set->mutex_list);
	set->values = NULL;
	set->mutex_list = NULL;
	free(set);

	temp = list;
	while (temp != NULL) {
		next = temp->next;
		temp->values = NULL;
		temp->mutex_list = NULL;
		free(temp);
		temp = next;
	}
}