#include "scheduling.h"
#include "init.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

static void job_one(t_setting *set, t_ready_queue *ready_queue, int time) {
	t_ready_queue *temp;
    t_ready_queue *shortest_job_prev = NULL;
    t_ready_queue *shortest_job = NULL;
    int shortest_burst_time = INT_MAX; // 큰 값으로 초기화
    int id;

    pthread_mutex_lock(set->mutex_list->ready_queue);
    ready_queue = set->values->ready_queue;

    if (ready_queue->next != NULL) {
		pthread_mutex_lock(set->mutex_list->p);
        printf("%ds : Monitor : ready queue list : ", time);
        for (temp = ready_queue->next; temp != NULL; temp = temp->next) {
            printf("%d, ", temp->id);
            if (temp->burst_time < shortest_burst_time) {
                shortest_burst_time = temp->burst_time;
                shortest_job_prev = ready_queue; // 이전 노드 저장
                shortest_job = temp;
            }
            ready_queue = temp; // 큐 순회
        }
        printf("\n");
		pthread_mutex_unlock(set->mutex_list->p);

        if (shortest_job != NULL) {
            id = shortest_job->id;

            // 리스트에서 shortest_job 제거
            if (shortest_job_prev != NULL) {
                shortest_job_prev->next = shortest_job->next;
            }

            free(shortest_job);

            pthread_mutex_unlock(set->mutex_list->ready_queue);
            pthread_mutex_lock(set->mutex_list->cpu);
            set->values->process_on_cpu = id;
            pthread_mutex_unlock(set->mutex_list->cpu);
            pthread_mutex_lock(set->mutex_list->p);
            printf("%ds : Monitor : %d loaded on cpu\n", set->values->time, id);
            pthread_mutex_unlock(set->mutex_list->p);
        } else {
            pthread_mutex_unlock(set->mutex_list->ready_queue);
            if (set->counter == set->total_process_count || set->values->remain_thread_count == 0) {
                return;
            }
            printf("%ds : Monitor : no process\n", set->values->time);
            pthread_mutex_lock(set->mutex_list->t);
            set->values->time++;
            pthread_mutex_unlock(set->mutex_list->t);
        }
    } else {
        pthread_mutex_unlock(set->mutex_list->ready_queue);
        if (set->counter == set->total_process_count || set->values->remain_thread_count == 0) {
            return;
        }
        printf("%ds : Monitor : no process\n", set->values->time);
        pthread_mutex_lock(set->mutex_list->t);
        set->values->time++;
        pthread_mutex_unlock(set->mutex_list->t);
    }
}
/*
static void job_two(t_setting *set, int running_id) {
	int remaining_time = -1;

	while (remaining_time == -1) {
		pthread_mutex_lock(set->mutex_list->r_t);
		remaining_time = set->values->remaining_time;
		pthread_mutex_unlock(set->mutex_list->r_t);
	}

	if (remaining_time == 0) {
		pthread_mutex_lock(set->mutex_list->p);
		printf("%ds : %d finished\n", set->values->time, running_id);
		pthread_mutex_unlock(set->mutex_list->p);
		pthread_mutex_lock(set->mutex_list->cpu);
		set->values->process_on_cpu = -1;
		// set->values->cpu_working = FALSE;
		pthread_mutex_unlock(set->mutex_list->cpu);
		pthread_mutex_lock(set->mutex_list->t);
		set->values->time = set->values->time + CONTEXT_SWITCH;
		pthread_mutex_unlock(set->mutex_list->t);
		pthread_mutex_lock(set->mutex_list->r_t);
		set->values->remaining_time = -1;
		pthread_mutex_unlock(set->mutex_list->r_t);
		set->counter++;
	} else {
		pthread_mutex_lock(set->mutex_list->cpu);
		set->values->cpu_working = TRUE;
		pthread_mutex_unlock(set->mutex_list->cpu);
		pthread_mutex_lock(set->mutex_list->t);
		set->values->time++;
		pthread_mutex_unlock(set->mutex_list->t);
		pthread_mutex_lock(set->mutex_list->r_t);
		set->values->remaining_time = -1;
		pthread_mutex_unlock(set->mutex_list->r_t);
	}
}
*/
void *sjf(void *arg) {
	t_setting	*set;
	t_ready_queue	*ready_queue;
	int remain_thread_count;
	int	running_id;
	int time;

	set = (t_setting *)arg;
	ready_queue = NULL;
	time = 0;

	wait_starting(set);

	printf("sjf start\n");
	while(1) {
		pthread_mutex_lock(set->mutex_list->check);
		remain_thread_count = set->values->remain_thread_count;
		pthread_mutex_unlock(set->mutex_list->check);
		if (set->counter == set->total_process_count || remain_thread_count == 0) {
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

		if (running_id == -1) {
			job_one(set, ready_queue, time);
		} else {
			job_two(set, running_id);
		}
		exit_routine(set);
	}
	pthread_mutex_lock(set->mutex_list->p);
	printf("sjf end\n");
	pthread_mutex_unlock(set->mutex_list->p);

	return (0);
}