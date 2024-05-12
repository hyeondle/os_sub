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

void	*cycle(void *arg) {
	t_process *p;
	p = (t_process *)arg;
	int time = 0;
	int response_time = -1;
	int turnaround_time;

	wait_starting(p);

	printf("cycle started\n");

	before_submit(p, &time);

	while (1) {
		if (p->remaining_time == 0) {
			// printer(p, FINISHED);
			turnaround_time = time - p->arrival_time;
			break;
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

		pthread_mutex_lock(p->mutex_list->t);
		time = p->values->time;
		pthread_mutex_unlock(p->mutex_list->t);
	}
	printer(p, TERMINATED, time);
	pthread_mutex_lock(p->mutex_list->p);
	// print all statement
	pthread_mutex_unlock(p->mutex_list->p);
	return (0);
}