#include "scheduling.h"
#include "init.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

static void job_one(t_setting *set, t_ready_queue *ready_queue) {
	t_ready_queue *temp;
	t_ready_queue *last;
	int id;

	pthread_mutex_lock(set->mutex_list->ready_queue);
	ready_queue = set->values->ready_queue;

	if (ready_queue->next != NULL) {
		printf("\nready_queue list : \n");
		printf("id : ");
		for (t_ready_queue *temp = ready_queue->next; temp != NULL; temp = temp->next) {
			printf("%d, ", temp->id);
		}
		printf("\n\n");

		temp = ready_queue->next;

		last = ready_queue;
		while (last->next != NULL) {
			last = last->next;
		}
		last->next = temp;
		ready_queue->next = temp->next;
		temp->next = NULL;
		temp = ready_queue->next;
		pthread_mutex_unlock(set->mutex_list->ready_queue);
		pthread_mutex_lock(set->mutex_list->cpu);
		set->values->process_on_cpu = temp->id;
		pthread_mutex_unlock(set->mutex_list->cpu);
		pthread_mutex_lock(set->mutex_list->p);
		printf("%ds : %d loaded on cpu\n", set->values->time, temp->id);
		pthread_mutex_unlock(set->mutex_list->p);
	} else {
		pthread_mutex_unlock(set->mutex_list->ready_queue);
		if (set->counter == set->total_process_count || set->values->remain_thread_count == 0) {
			return ;
		}
		printf("%ds : no process\n", set->values->time);
		pthread_mutex_lock(set->mutex_list->t);
		set->values->time++;
		pthread_mutex_unlock(set->mutex_list->t);
	}
}

static void job_two_c(t_setting *set, int running_id) {
	int remaining_time = -1;

	while (remaining_time == -1) {
		pthread_mutex_lock(set->mutex_list->t);
		remaining_time = set->values->remaining_time;
		pthread_mutex_unlock(set->mutex_list->t);
	}

	if (remaining_time == 0) {
		pthread_mutex_lock(set->mutex_list->p);
		printf("%ds : %d finished\n", set->values->time, running_id);
		pthread_mutex_unlock(set->mutex_list->p);
		pthread_mutex_lock(set->mutex_list->cpu);
		set->values->process_on_cpu = -1;
		set->values->cpu_working = FALSE;
		pthread_mutex_unlock(set->mutex_list->cpu);
		pthread_mutex_lock(set->mutex_list->t);
		set->values->time = set->values->time + CONTEXT_SWITCH;
		pthread_mutex_unlock(set->mutex_list->t);
		pthread_mutex_lock(set->mutex_list->r_t);
		set->values->remaining_time = -1;
		pthread_mutex_unlock(set->mutex_list->r_t);
		set->counter++;

		pthread_mutex_lock(set->mutex_list->ready_queue);
		t_ready_queue *prev = set->values->ready_queue;
		t_ready_queue *curr = prev->next;
		while (curr != NULL) {
			if (curr->id == running_id) {
				prev->next = curr->next;
				free(curr);
				break;
			}
			prev = curr;
			curr = curr->next;
		}
		pthread_mutex_unlock(set->mutex_list->ready_queue);

	} else {
		pthread_mutex_lock(set->mutex_list->cpu);
		set->values->cpu_working = TRUE;
		pthread_mutex_unlock(set->mutex_list->cpu);
		pthread_mutex_lock(set->mutex_list->t);
		set->values->time++;
		set->values->loaded_process_execution_time++;
		pthread_mutex_unlock(set->mutex_list->t);
		pthread_mutex_lock(set->mutex_list->r_t);
		set->values->remaining_time = -1;
		pthread_mutex_unlock(set->mutex_list->r_t);
	}
}

void *round_robin(void *arg) {
	t_setting *set;
	t_ready_queue *ready_queue;
	int running_id = -1;
	int execution_time = 0;

	set = (t_setting *)arg;
	ready_queue = NULL;

	wait_starting(set);

	printf("round_robin start\n");
	while (1) {
		if (set->values->remain_thread_count == 0 || set->counter == set->total_process_count) {
			break;
		}
		wait_routine(set);
		usleep(100);

		pthread_mutex_lock(set->mutex_list->cpu);
		running_id = set->values->process_on_cpu;
		pthread_mutex_unlock(set->mutex_list->cpu);
		pthread_mutex_lock(set->mutex_list->t);
		execution_time = set->values->loaded_process_execution_time;
		pthread_mutex_unlock(set->mutex_list->t);

		if (execution_time == QUANTUM || running_id == -1) {
			job_one(set, ready_queue);
			pthread_mutex_lock(set->mutex_list->t);
			set->values->loaded_process_execution_time = 0;
			pthread_mutex_unlock(set->mutex_list->t);
		} else {
			job_two_c(set, running_id);
		}
		exit_routine(set);
	}
	pthread_mutex_lock(set->mutex_list->p);
	printf("round_robin end\n");
	pthread_mutex_unlock(set->mutex_list->p);

	return (0);
}