#include "scheduling.h"
#include "init.h"
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

t_fcfs	*init_fcfs(t_setting *set) {
	t_fcfs	*fcfs;

	fcfs = (t_fcfs *)malloc(sizeof(t_fcfs));
	if (fcfs == NULL) {
		// error
		return NULL;
	}
	fcfs->time = 0;
	fcfs->counter = 0;
	fcfs->running = 0;
	fcfs->done = 0;
	fcfs->total_waiting_time = 0;
	fcfs->total_turnaround_time = 0;
	fcfs->avg_waiting_time = 0;
	fcfs->avg_turnaround_time = 0;
	return fcfs;
}

static void	wait_starting(t_setting *set) {
	while (1) {
		pthread_mutex_lock(set->mutex_list->cpu);
		if (set->values->thread_count == 0) {
			pthread_mutex_unlock(set->mutex_list->cpu);
			break;
		}
		pthread_mutex_unlock(set->mutex_list->cpu);
		sleep(1);
	}
}

void	*fcfs(void *arg) {
	t_setting	*set;
	t_fcfs		*fcfs;
	t_ready_queue	*ready_queue;
	t_ready_queue	*temp;
	int id;
	int burst_time;

	set = (t_setting *)arg;
	fcfs = init_fcfs(set);
	if (fcfs == NULL) {
		// error
		return NULL;
	}

	printf("fcfs initialized\n");

	wait_starting(set);

	printf("fcfs started\n");
	printf("%d\n", set->total_process_count);
	printf("%d\n", set->values->thread_count);
	sleep(1);
	while (fcfs->counter < set->total_process_count) {
		pthread_mutex_lock(set->mutex_list->ready_queue);
		ready_queue = set->values->ready_queue;
		if (ready_queue->next != NULL) {
			temp = ready_queue->next;
			id = temp->id;
			burst_time = temp->burst_time;

			ready_queue->next = temp->next;
			free(temp);
		} else {
			pthread_mutex_unlock(set->mutex_list->ready_queue);
			printf("%d : no process\n", set->values->time);
			pthread_mutex_lock(set->mutex_list->t);
			set->values->time++;
			pthread_mutex_unlock(set->mutex_list->t);
			sleep(1);
			continue;
		}
		pthread_mutex_unlock(set->mutex_list->ready_queue);

		pthread_mutex_lock(set->mutex_list->cpu);
		set->values->process_on_cpu = id;
		printf("%d : %d started\n", set->values->time, id);
		pthread_mutex_unlock(set->mutex_list->cpu);


		//print
		pthread_mutex_lock(set->mutex_list->p);
		printf("%d : %d loaded on cpu\n", set->values->time, id);
		pthread_mutex_unlock(set->mutex_list->p);

		for (int i = 0; i < burst_time; i++) {
			pthread_mutex_lock(set->mutex_list->t);
			set->values->time++;
			pthread_mutex_unlock(set->mutex_list->t);
			sleep(1);
		}

		pthread_mutex_lock(set->mutex_list->cpu);
		set->values->process_on_cpu = -1;
		pthread_mutex_unlock(set->mutex_list->cpu);

		//print
		pthread_mutex_lock(set->mutex_list->p);
		printf("%d : %d finished\n", set->values->time, id);
		pthread_mutex_unlock(set->mutex_list->p);

		fcfs->counter++;

		// id = temp->id;
		// burst_time = temp->burst_time;
		// if (temp->next != NULL)
		// 	ready_queue->next = temp->next;
		// else
		// 	ready_queue->next = NULL;
		// free(temp);
		// pthread_mutex_unlock(set->mutex_list->ready_queue);
		// pthread_mutex_lock(set->mutex_list->cpu);
		// set->values->process_on_cpu = id;
		// pthread_mutex_unlock(set->mutex_list->cpu);
		// for (int i = 0; i < burst_time; i++) {
		// 	pthread_mutex_lock(set->mutex_list->t);
		// 	set->values->time++;
		// 	pthread_mutex_unlock(set->mutex_list->t);
		// }
		// pthread_mutex_lock(set->mutex_list->cpu);
		// set->values->process_on_cpu = -1;
		// pthread_mutex_unlock(set->mutex_list->cpu);
	}

	// free(fcfs);
	// print
	pthread_mutex_lock(set->mutex_list->p);
	printf("fcfs done\n");
	printf("total_waiting_time : %d\n", fcfs->total_waiting_time);
	printf("total_turnaround_time : %d\n", fcfs->total_turnaround_time);
	printf("avg_waiting_time : %f\n", fcfs->avg_waiting_time);
	printf("avg_turnaround_time : %f\n", fcfs->avg_turnaround_time);
	pthread_mutex_unlock(set->mutex_list->p);

	return (0);
}