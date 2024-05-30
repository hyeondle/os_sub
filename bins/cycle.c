#include "scheduling.h"
#include "init.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void	process_entered(t_process *process) {
	pthread_mutex_lock(process->mutex_list->cpu);
	process->values->thread_count--;
	pthread_mutex_unlock(process->mutex_list->cpu);
}

static void	wait_p_starting(t_process *process) {
	process_entered(process);
	while(1) {
		pthread_mutex_lock(process->mutex_list->cpu);
		if (process->values->thread_count == 0) {
			pthread_mutex_unlock(process->mutex_list->cpu);
			break;
		}
		pthread_mutex_unlock(process->mutex_list->cpu);
		sleep(1);
	}
}

void arrival(t_process *process, int time) {
	t_ready_queue *new;
	t_ready_queue *temp;

	new = (t_ready_queue *)malloc(sizeof(t_ready_queue));
	new->id = process->id;
	new->priority = process->priority;
	new->burst_time = process->burst_time;
	new->arrival_time = process->arrival_time;
	new->remaining_time = process->burst_time;
	new->next = NULL;

	pthread_mutex_lock(process->mutex_list->ready_queue);
	temp = process->values->ready_queue;
	while (temp->next != NULL) {
		temp = temp->next;
	}
	temp->next = new;
	pthread_mutex_unlock(process->mutex_list->ready_queue);

	process->submitted = TRUE;
	pthread_mutex_lock(process->mutex_list->p);
	printf("%ds : Process %d is submitted\n", time, process->id);
	pthread_mutex_unlock(process->mutex_list->p);
}

void	enter_p_rountine(t_process *p) {
	pthread_mutex_lock(p->mutex_list->check);
	p->values->checked_count++;
	pthread_mutex_unlock(p->mutex_list->check);
	while (1) {
		pthread_mutex_lock(p->mutex_list->check);
		if (p->values->routine == TRUE) {
			pthread_mutex_unlock(p->mutex_list->check);
			break;
		}
		pthread_mutex_unlock(p->mutex_list->check);
	}
}

static void	exit_p_routine(t_process *p) {
	pthread_mutex_lock(p->mutex_list->check);
	p->values->checked_count2++;
	pthread_mutex_unlock(p->mutex_list->check);
	while (1) {
		pthread_mutex_lock(p->mutex_list->check);
		if (p->values->routine == FALSE) {
			pthread_mutex_unlock(p->mutex_list->check);
			break;
		}
		pthread_mutex_unlock(p->mutex_list->check);
	}
}

int time_check(t_process *p, int time) {
	int now = -1;
	while (now != time) {
		pthread_mutex_lock(p->mutex_list->t);
		now = p->values->time;
		pthread_mutex_unlock(p->mutex_list->t);
	}
	return now;
}

void	*cycle(void *arg) {
	t_process *p;
	p = (t_process *)arg;
	int time = -1;
	int prev_time = 0;
	int process_on_cpu = 0;

	wait_p_starting(p);
	while (1) {
		if (p->remaining_time == -1) {
			printf("exit process\n");
			pthread_mutex_lock(p->mutex_list->t);
			time = p->values->time;
			pthread_mutex_unlock(p->mutex_list->t);
			p->turnaround_time = prev_time - p->arrival_time;
			pthread_mutex_lock(p->mutex_list->check);
			p->values->remain_thread_count--;
			pthread_mutex_unlock(p->mutex_list->check);
			break;
		}

		enter_p_rountine(p);

		// time = time_check(p, time);

		pthread_mutex_lock(p->mutex_list->t);
		time = p->values->time;
		pthread_mutex_unlock(p->mutex_list->t);

		if (p->submitted == FALSE) {

			// pthread_mutex_lock(p->mutex_list->t);
			// time = p->values->time;
			// pthread_mutex_unlock(p->mutex_list->t);

			if (time == p->arrival_time) {
				arrival(p, time);
				prev_time = time;
			}
			exit_p_routine(p);
			continue;
		}

		pthread_mutex_lock(p->mutex_list->cpu);
		process_on_cpu = p->values->process_on_cpu;
		pthread_mutex_unlock(p->mutex_list->cpu);

		if (process_on_cpu != p->id) {
			pthread_mutex_lock(p->mutex_list->cpu);
			if (p->values->cpu_working == TRUE && p->submitted == TRUE) {
				p->waiting_time = p->waiting_time + (time - prev_time);
				pthread_mutex_lock(p->mutex_list->p);
				printf("%ds : Process %d is waiting\n", time, p->id);
				pthread_mutex_unlock(p->mutex_list->p);
			}
			pthread_mutex_unlock(p->mutex_list->cpu);
		} else {
			if (p->response_time == -1) {
				pthread_mutex_lock(p->mutex_list->p);
				printf("%ds : Process %d is running\n", time, p->id);
				pthread_mutex_unlock(p->mutex_list->p);
				p->response_time = time - p->arrival_time;
			}
			pthread_mutex_lock(p->mutex_list->r_t);
			p->values->remaining_time = p->remaining_time;
			pthread_mutex_unlock(p->mutex_list->r_t);
			p->remaining_time--;
		}

		prev_time = time;

		exit_p_routine(p);
	}

	pthread_mutex_lock(p->mutex_list->p);
	printf("Process_ %d Terminated\n", p->id);
	printf("Process_ %d Waiting Time : %d\n", p->id, p->waiting_time);
	printf("Process_ %d Turnaround Time : %d\n", p->id, p->turnaround_time);
	printf("Process_ %d Response Time : %d\n", p->id, p->response_time);
	pthread_mutex_unlock(p->mutex_list->p);
	return (0);
}

