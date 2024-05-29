#include "scheduling.h"
#include "init.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

static void job_one(t_setting *set, t_ready_queue *ready_queue, int *running_id, int time) {
    t_ready_queue *temp;
    // t_ready_queue *shortest_job_prev = NULL;
    t_ready_queue *shortest_job = NULL;
    int shortest_remaining_time = INT_MAX;
    int id = -1;

    pthread_mutex_lock(set->mutex_list->ready_queue);
    ready_queue = set->values->ready_queue;

    if (ready_queue->next != NULL) {
        pthread_mutex_lock(set->mutex_list->p);
        printf("%ds : Monitor : ready queue list : ", time);
        for (temp = ready_queue->next; temp != NULL; temp = temp->next) {
            printf("%d, ", temp->id);

            // Check for remaining_time and choose the shortest one
            if (temp->remaining_time < shortest_remaining_time) {
                shortest_remaining_time = temp->remaining_time;
                // shortest_job_prev = ready_queue;
                shortest_job = temp;
            } else if (temp->remaining_time == shortest_remaining_time) {
                // If the remaining times are the same, keep the previous job
                if (shortest_job == NULL || temp->arrival_time < shortest_job->arrival_time) {
                    // shortest_job_prev = ready_queue;
                    shortest_job = temp;
                }
            }
            ready_queue = temp;
        }
        printf("\n");
        pthread_mutex_unlock(set->mutex_list->p);

        if (shortest_job != NULL) {
            id = shortest_job->id;
            pthread_mutex_lock(set->mutex_list->cpu);
            set->values->process_on_cpu = id;
            set->values->cpu_working = TRUE;
            pthread_mutex_unlock(set->mutex_list->cpu);
            pthread_mutex_lock(set->mutex_list->r_t);
            set->values->remaining_time = shortest_job->remaining_time;
            pthread_mutex_unlock(set->mutex_list->r_t);
            pthread_mutex_lock(set->mutex_list->p);
			if (*running_id != id && *running_id != -1)
				printf("%ds : Monitor : %d switched out\n", set->values->time, *running_id);
			if (shortest_job->remaining_time != 0)
				printf("%ds : Monitor : %d loaded on cpu\n", set->values->time, id);
            pthread_mutex_unlock(set->mutex_list->p);
        }
    }
    pthread_mutex_unlock(set->mutex_list->ready_queue);
	*running_id = id;
}

static void job_two_c(t_setting *set, int running_id, int time) {
    if (running_id == -1) {
        pthread_mutex_lock(set->mutex_list->p);
		if (set->counter != set->total_process_count)
			printf("%ds : Monitor : no process\n", set->values->time);
        pthread_mutex_unlock(set->mutex_list->p);
        pthread_mutex_lock(set->mutex_list->t);
        set->values->time++;
        pthread_mutex_unlock(set->mutex_list->t);
        return;
    }

    pthread_mutex_lock(set->mutex_list->r_t);
    int remaining_time = set->values->remaining_time;
    pthread_mutex_unlock(set->mutex_list->r_t);

	if (remaining_time == -1) {
		printf("%ds : Monitor : %d delete\n", time, running_id);
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
		set->counter++;
		return ;
	}

    if (remaining_time == 0) {
        pthread_mutex_lock(set->mutex_list->p);
        printf("%ds : Monitor : %d finished\n", set->values->time, running_id);
        pthread_mutex_unlock(set->mutex_list->p);
        pthread_mutex_lock(set->mutex_list->cpu);
        set->values->process_on_cpu = -1;
        set->values->cpu_working = FALSE;
        pthread_mutex_unlock(set->mutex_list->cpu);
        pthread_mutex_lock(set->mutex_list->t);
        set->values->time += CONTEXT_SWITCH;
        pthread_mutex_unlock(set->mutex_list->t);
        pthread_mutex_lock(set->mutex_list->r_t);
        set->values->remaining_time = -1;
        pthread_mutex_unlock(set->mutex_list->r_t);
        // set->counter++;
		pthread_mutex_lock(set->mutex_list->ready_queue);
        t_ready_queue *temp = set->values->ready_queue->next;
        while (temp != NULL) {
            if (temp->id == running_id) {
                temp->remaining_time--;
                break;
            }
            temp = temp->next;
        }
        pthread_mutex_unlock(set->mutex_list->ready_queue);
    } else {
        pthread_mutex_lock(set->mutex_list->p);
        printf("%ds : Monitor : %d is working\n", set->values->time, running_id);
        pthread_mutex_unlock(set->mutex_list->p);
        pthread_mutex_lock(set->mutex_list->cpu);
        set->values->cpu_working = TRUE;
        set->values->process_on_cpu = running_id;
        pthread_mutex_unlock(set->mutex_list->cpu);
        pthread_mutex_lock(set->mutex_list->t);
        set->values->time++;
        pthread_mutex_unlock(set->mutex_list->t);

        // Decrease remaining_time of the running process in the ready_queue
        pthread_mutex_lock(set->mutex_list->ready_queue);
        t_ready_queue *temp = set->values->ready_queue->next;
        while (temp != NULL) {
            if (temp->id == running_id) {
                temp->remaining_time--;
                break;
            }
            temp = temp->next;
        }
        pthread_mutex_unlock(set->mutex_list->ready_queue);
    }
	// pthread_mutex_lock(set->mutex_list->r_t);
    // set->values->remaining_time--;
    // pthread_mutex_unlock(set->mutex_list->r_t);
}

void *srtf(void *arg) {
    t_setting *set;
    t_ready_queue *ready_queue;
    int running_id = -1;
    int remain_thread_count;
    int time;

    set = (t_setting *)arg;
    ready_queue = NULL;
    time = 0;

    wait_starting(set);

    printf("SRTF scheduling\n");
    while (1) {
        pthread_mutex_lock(set->mutex_list->check);
        remain_thread_count = set->values->remain_thread_count;
        pthread_mutex_unlock(set->mutex_list->check);
        if (remain_thread_count == 0) {
            break;
        }
        wait_routine(set);
        usleep(100);

        pthread_mutex_lock(set->mutex_list->cpu);
		running_id = set->values->process_on_cpu;
		pthread_mutex_unlock(set->mutex_list->cpu);
        pthread_mutex_lock(set->mutex_list->t);
        time = set->values->time;
        pthread_mutex_unlock(set->mutex_list->t);

        job_one(set, ready_queue, &running_id, time);
        job_two_c(set, running_id, time);

        exit_routine(set);
		if (remain_thread_count == 0) {
            break;
        }
    }
    pthread_mutex_lock(set->mutex_list->p);
    printf("SRTF scheduling end\n");
    pthread_mutex_unlock(set->mutex_list->p);

    return (0);
}