#include "scheduling.h"
#include "init.h"
#include <stdlib.h>

void	process_entered(t_process *process) {
	pthread_mutex_lock(process->mutex_list->t);
	process->values->thread_count--;
	pthread_mutex_unlock(process->mutex_list->t);
}

void	wait_starting(t_process *process) {
	process_entered(process);
	while(1) {
		pthread_mutex_lock(process->mutex_list->cpu);
		if (process->values->thread_count == 0) {
			pthread_mutex_unlock(process->mutex_list->cpu);
			break;
		}
		pthread_mutex_unlock(process->mutex_list->cpu);
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
	while (temp->next != NULL) {
		temp = temp->next;
	}
	temp->next = new;
	pthread_mutex_unlock(process->mutex_list->ready_queue);
	process->submitted = TRUE;
}

void	before_submit(t_process *p, int *time) {
	while (1) {
		time_check(p, time);
		if (*time == p->arrival_time && p->submitted == FALSE) {
			arrival(p);
			printer(p, READY);
			break;
		}
	}
}

void	*cycle(void *arg) {
	t_process *p;
	p = (t_process *)arg;
	int time;
	int time2;
	int response_time;
	int turnaround_time;

	time = 0;
	wait_starting(p);
	before_submit(p, &time);
	time2 = time;
	while (1) {
		if (p->remaining_time == 0) {
			// printer(p, FINISHED);
			turnaround_time = time - p->arrival_time;
			break;
		}
		while (time > time2)
			time_check(p, &time2);
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
	}
	pthread_mutex_lock(p->mutex_list->p);
	// print all statement
	pthread_mutex_unlock(p->mutex_list->p);
	return (0);
}