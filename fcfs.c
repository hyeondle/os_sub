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

void	wait_rountine(t_setting *set) {
	while (1) {
		pthread_mutex_lock(set->mutex_list->check);
		if (set->values->checked_count == set->values->remain_thread_count) {
			set->values->routine = TRUE;
			set->values->checked_count = 0;
			pthread_mutex_unlock(set->mutex_list->check);
			break;
		}
		pthread_mutex_unlock(set->mutex_list->check);
		usleep(100);
	}
}

static void	exit_routine(t_setting *set) {
	while (1) {
		pthread_mutex_lock(set->mutex_list->check);
		if (set->values->checked_count2 == set->values->remain_thread_count) {
			set->values->routine = FALSE;
			set->values->checked_count2 = 0;
			pthread_mutex_unlock(set->mutex_list->check);
			break;
		}
		pthread_mutex_unlock(set->mutex_list->check);
		usleep(100);
	}
}

static void job_one(t_setting *set) {

}

static void job_two(t_setting *set) {

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

	wait_starting(set);

	printf("fcfs start\n");
	while (1) {
		if (fcfs->counter == set->total_process_count || set->values->remain_thread_count == 0)
			break;
		wait_rountine(set);
		// printf("fcfs routine\n");
		usleep(100);

		// pthread_mutex_lock(set->mutex_list->cpu);
		// if (set->values->loaded_process_execution_time == -1) {
		// 	pthread_mutex_unlock(set->mutex_list->cpu);
		// 	job_one(set);
		// } else {
		// 	pthread_mutex_unlock(set->mutex_list->cpu);
		// 	job_two(set);
		// }
		pthread_mutex_lock(set->mutex_list->ready_queue);
		ready_queue = set->values->ready_queue;
		if (ready_queue->next != NULL) {
			temp = ready_queue->next;
			id = temp->id;
			burst_time = temp->burst_time;

			ready_queue->next = temp->next;
			free(temp);
		} else {
			if (fcfs->counter == set->total_process_count || set->values->remain_thread_count == 0) {
				pthread_mutex_unlock(set->mutex_list->ready_queue);
				exit_routine(set);
				break;
			}
			pthread_mutex_unlock(set->mutex_list->ready_queue);
			printf("%d : no process\n", set->values->time);
			pthread_mutex_lock(set->mutex_list->t);
			set->values->time++;
			pthread_mutex_unlock(set->mutex_list->t);
			exit_routine(set);
			usleep(100);
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

		pthread_mutex_lock(set->mutex_list->t);
		set->values->time = set->values->time + burst_time;
		pthread_mutex_unlock(set->mutex_list->t);


		pthread_mutex_lock(set->mutex_list->cpu);
		set->values->process_on_cpu = -1;
		pthread_mutex_unlock(set->mutex_list->cpu);

		//print
		pthread_mutex_lock(set->mutex_list->p);
		printf("%d : %d finished\n", set->values->time, id);
		pthread_mutex_unlock(set->mutex_list->p);

		fcfs->counter++;

		// printf("fcfs routine end\n");

		exit_routine(set);
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
	// printf("total_waiting_time : %d\n", fcfs->total_waiting_time);
	// printf("total_turnaround_time : %d\n", fcfs->total_turnaround_time);
	// printf("avg_waiting_time : %f\n", fcfs->avg_waiting_time);
	// printf("avg_turnaround_time : %f\n", fcfs->avg_turnaround_time);
	pthread_mutex_unlock(set->mutex_list->p);

	return (0);
}