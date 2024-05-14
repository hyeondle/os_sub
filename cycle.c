#include "scheduling.h"
#include "init.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void	process_entered(t_process *process) {
	pthread_mutex_lock(process->mutex_list->cpu);
	process->values->thread_count--;
	pthread_mutex_unlock(process->mutex_list->cpu);
}

static void	wait_starting(t_process *process) {
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

void time_check(t_process *process, int *time) {
	pthread_mutex_lock(process->mutex_list->t);
	*time = process->values->time;
	pthread_mutex_unlock(process->mutex_list->t);
}

void arrival(t_process *process) {
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
	printf("Process %d submitted\n", process->id);
	pthread_mutex_unlock(process->mutex_list->p);
}

void	before_submit(t_process *p, int *time) {
	while (1) {
		time_check(p, time);
		if (*time == p->arrival_time && p->submitted == FALSE) {
			arrival(p);
			// printer(p, READY);
			break;
		}
	}
}

void	enter_rountine(t_process *p) {
	// printf("enter routine\n");
	pthread_mutex_lock(p->mutex_list->check);
	p->values->checked_count++;
	pthread_mutex_unlock(p->mutex_list->check);
	// printf("checked count");
}

static void	exit_routine(t_process *p) {
	pthread_mutex_lock(p->mutex_list->check);
	p->values->checked_count2++;
	pthread_mutex_unlock(p->mutex_list->check);
}

void	wait_enter_routine(t_process *p) {
	while (1) {
		pthread_mutex_lock(p->mutex_list->check);
		if (p->values->routine == TRUE) {
			pthread_mutex_unlock(p->mutex_list->check);
			break;
		}
		pthread_mutex_unlock(p->mutex_list->check);
	}
}

void	wait_exit_routine(t_process *p) {
	while (1) {
		pthread_mutex_lock(p->mutex_list->check);
		if (p->values->routine == FALSE) {
			pthread_mutex_unlock(p->mutex_list->check);
			break;
		}
		pthread_mutex_unlock(p->mutex_list->check);
	}
}

void	*cycle(void *arg) {
	t_process *p;
	p = (t_process *)arg;
	int time = 0;
	int response_time = -1;

	int turnaround_time;

	wait_starting(p);

	printf("cycle started\n");

	// before_submit(p, &time);

	while (1) {
		if (p->remaining_time == 0) {
			// printer(p, FINISHED);
			printf("exit process\n");
			turnaround_time = time - p->arrival_time;
			pthread_mutex_lock(p->mutex_list->check);
			p->values->remain_thread_count--;
			pthread_mutex_unlock(p->mutex_list->check);
			break;
		}

		enter_rountine(p);
		wait_enter_routine(p);

		// printf("rountine start\n");

		pthread_mutex_lock(p->mutex_list->t);
		time = p->values->time;
		pthread_mutex_unlock(p->mutex_list->t);

		if (p->submitted == FALSE) {
			if (time == p->arrival_time) {
				arrival(p);
			}
			exit_routine(p);
			wait_exit_routine(p);
			continue;
		}

		pthread_mutex_lock(p->mutex_list->cpu);
		if (p->values->process_on_cpu == p->id) {
			if (response_time == -1) {
				response_time = time - p->arrival_time;
			}
			p->remaining_time--;
		} else {
			p->waiting_time++;
		}
		pthread_mutex_unlock(p->mutex_list->cpu);

		exit_routine(p);
		wait_exit_routine(p);
	}

	pthread_mutex_lock(p->mutex_list->p);
	// print all statement
	printf("Process %d Terminated\n", p->id);
	pthread_mutex_unlock(p->mutex_list->p);
	return (0);
}