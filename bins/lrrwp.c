//Limited Round Robin with Priority

#include "scheduling.h"
#include "init.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

static t_ready_queue *create_running_queue(void) {
	t_ready_queue *running_queue;

	running_queue = (t_ready_queue *)malloc(sizeof(t_ready_queue));
	if (running_queue == NULL) {
		printf("malloc failed\n");
		exit(1);
	}
	running_queue->next = NULL;
	running_queue->id = -1;
	running_queue->priority = -1;

	return (running_queue);
}

static void update_priority(t_setting *set, t_ready_queue *ready_queue) {
	t_ready_queue *temp;
	t_process *target;
	int id;

	temp = ready_queue->next;
	while (temp != NULL) {
		id = temp->id;
		pthread_mutex_lock(set->mutex_list->cpu); // if fsanitize error, don't use this lock/unlock
		target = set->processes->next;
		while (target != NULL) {
			if (target->id == id) {
				if (target->remaining_time <= (2 * QUANTUM))
					temp->priority = INT_MAX;
				else
					temp->priority = target->waiting_time + target->remaining_time;
				break ;
			}
			target = target->next;
		}
		pthread_mutex_unlock(set->mutex_list->cpu);
		temp = temp->next;
	}
}

static void job_one(t_setting *set, t_ready_queue **ready_queue, t_ready_queue **job, t_ready_queue *running_queue, int time) {
    t_ready_queue *temp;
    t_ready_queue *last;
    t_ready_queue *c_job;
    int id = -1;
    int count = 0;
    int priority = INT_MIN;

    pthread_mutex_lock(set->mutex_list->ready_queue);
    *ready_queue = set->values->ready_queue;
    c_job = *job;

    last = running_queue;
    while (last->next != NULL) {
        last = last->next;
        count++;
    }

    last->next = c_job;
    if (c_job != NULL)
        count++;

    if ((*ready_queue)->next != NULL) {
        pthread_mutex_lock(set->mutex_list->p);
        printf("%ds : ready_queue list : ", time);
        for (t_ready_queue *temp = (*ready_queue)->next; temp != NULL; temp = temp->next) {
            printf("%d, ", temp->id);
        }
        printf("\n");
        printf("%ds : running_queue list : ", time);
        for (t_ready_queue *temp = running_queue->next; temp != NULL; temp = temp->next) {
            printf("%d, ", temp->id);
        }
        printf("\n");
        pthread_mutex_unlock(set->mutex_list->p);

        if (count < RQ_SIZE) {
            update_priority(set, *ready_queue);

            temp = (*ready_queue)->next;
            while (temp != NULL) {
                if (temp->priority > priority) {
                    priority = temp->priority;
                    id = temp->id;
                }
                temp = temp->next;
            }

            temp = *ready_queue;
            while (temp->next != NULL && temp->next->id != id) {
                temp = temp->next;
            }

            if (temp->next != NULL) {
                t_ready_queue *selected_job = temp->next;
                temp->next = temp->next->next;

                last = running_queue;
                while (last->next != NULL) {
                    last = last->next;
                }

                last->next = selected_job;
                selected_job->next = NULL;
            }
        }
    }
    if (running_queue->next != NULL) {
        *job = running_queue->next;
        temp = *job;

        running_queue->next = temp->next;
        temp->next = NULL;

        id = temp->id;

        pthread_mutex_unlock(set->mutex_list->ready_queue);
        pthread_mutex_lock(set->mutex_list->cpu);
        set->values->process_on_cpu = id;
        pthread_mutex_unlock(set->mutex_list->cpu);
        pthread_mutex_lock(set->mutex_list->p);
        printf("%ds : Monitor : %d loaded on cpu\n", time, id);
        pthread_mutex_unlock(set->mutex_list->p);
    } else {
        pthread_mutex_unlock(set->mutex_list->ready_queue);
        if (set->counter == set->total_process_count || set->values->remain_thread_count == 0) {
            return ;
        }
        printf("%ds : Monitor : no process\n", time);
        pthread_mutex_lock(set->mutex_list->t);
        set->values->time++;
        pthread_mutex_unlock(set->mutex_list->t);
    }
}

static void job_two_c(t_setting *set, int running_id, t_ready_queue **job, int time) {
	int remaining_time = -1;
	int flag = 0;

	while (remaining_time == -1) {
		pthread_mutex_lock(set->mutex_list->r_t);
		remaining_time = set->values->remaining_time;
		pthread_mutex_unlock(set->mutex_list->r_t);
	}

	if (remaining_time == 0) {
		pthread_mutex_lock(set->mutex_list->p);
		printf("%ds : Monitor : %d is finished\n", time, running_id);
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

		free(*job);
		*job = NULL;
	} else {
		pthread_mutex_lock(set->mutex_list->p);
		printf("%ds : Monitor : %d is running\n", time, running_id);
		pthread_mutex_unlock(set->mutex_list->p);
		pthread_mutex_lock(set->mutex_list->cpu);
		set->values->cpu_working = TRUE;
		pthread_mutex_unlock(set->mutex_list->cpu);
		pthread_mutex_lock(set->mutex_list->t);
		set->values->time++;
		set->values->loaded_process_execution_time++;
		if (set->values->loaded_process_execution_time == QUANTUM) {
			flag = 1;
		}
		pthread_mutex_unlock(set->mutex_list->t);
		pthread_mutex_lock(set->mutex_list->r_t);
		set->values->remaining_time = -1;
		pthread_mutex_unlock(set->mutex_list->r_t);
		if (flag == 1) {
			pthread_mutex_lock(set->mutex_list->cpu);
			set->values->process_on_cpu = -1;
			set->values->cpu_working = FALSE;
			pthread_mutex_unlock(set->mutex_list->cpu);
		}
	}
}

void *lrrwp(void *arg) {
	t_setting *set;
	t_ready_queue *ready_queue;
	t_ready_queue *running_queue;
	t_ready_queue *job;
	int running_id = -2;
	int execution_time = 0;
	int remain_thread_count;
	int time = 0;

	set = (t_setting *)arg;
	ready_queue = NULL;
	job = NULL;
	running_queue = create_running_queue();

	wait_starting(set);

	printf("lrrwp start\n");
	while (1) {
		pthread_mutex_lock(set->mutex_list->check);
		remain_thread_count = set->values->remain_thread_count;
		pthread_mutex_unlock(set->mutex_list->check);
		if (remain_thread_count == 0 || set->counter == set->total_process_count) {
			break ;
		}
		wait_routine(set);
		usleep(100);

		pthread_mutex_lock(set->mutex_list->cpu);
		running_id = set->values->process_on_cpu;
		pthread_mutex_unlock(set->mutex_list->cpu);
		pthread_mutex_lock(set->mutex_list->t);
		execution_time = set->values->loaded_process_execution_time;
		time = set->values->time;
		pthread_mutex_unlock(set->mutex_list->t);
		if (execution_time == QUANTUM || running_id == -1) {
			job_one(set, &ready_queue, &job, running_queue, time);
			pthread_mutex_lock(set->mutex_list->t);
			set->values->loaded_process_execution_time = 0;
			pthread_mutex_unlock(set->mutex_list->t);
		} else {
			job_two_c(set, running_id, &job, time);
		}
		exit_routine(set);
	}
	pthread_mutex_lock(set->mutex_list->p);
	printf("lrrwp end\n");
	pthread_mutex_unlock(set->mutex_list->p);

	return (0);
}