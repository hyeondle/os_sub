#include "scheduling.h"
#include "init.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

// static void job_one(t_setting *set, t_ready_queue *ready_queue, t_ready_queue *job) {
//     t_ready_queue *temp;
//     t_ready_queue *last;
//     t_ready_queue *c_job;
//     int id;

//     pthread_mutex_lock(set->mutex_list->ready_queue);
//     ready_queue = set->values->ready_queue;
//     c_job = job;
//     if (ready_queue->next != NULL) {
//         printf("\nready_queue list : \n");
//         printf("id : ");
//         for (t_ready_queue *temp = ready_queue->next; temp != NULL; temp = temp->next) {
//             printf("%d, ", temp->id);
//         }
//         printf("\n\n");

// 		job = ready_queue->next;
//         temp = job;

//         ready_queue->next = temp->next;
//         temp->next = NULL;

//         last = ready_queue;
//         while (last->next != NULL) {
//             last = last->next;
//         }
//         last->next = c_job;

//         id = job->id;

//         pthread_mutex_unlock(set->mutex_list->ready_queue);
//         pthread_mutex_lock(set->mutex_list->cpu);
//         set->values->process_on_cpu = id;
//         pthread_mutex_unlock(set->mutex_list->cpu);
//         pthread_mutex_lock(set->mutex_list->p);
//         printf("%ds : %d loaded on cpu\n", set->values->time, id);
//         pthread_mutex_unlock(set->mutex_list->p);
//     } else {
//         pthread_mutex_unlock(set->mutex_list->ready_queue);
//         if (set->counter == set->total_process_count || set->values->remain_thread_count == 0) {
//             return ;
//         }
//         printf("%ds : no process\n", set->values->time);
//         pthread_mutex_lock(set->mutex_list->t);
//         set->values->time++;
//         pthread_mutex_unlock(set->mutex_list->t);
//     }
// }


// static void job_two_c(t_setting *set, int running_id, t_ready_queue *job) {
// 	int remaining_time = -1;

// 	while (remaining_time == -1) {
// 		pthread_mutex_lock(set->mutex_list->t);
// 		remaining_time = set->values->remaining_time;
// 		pthread_mutex_unlock(set->mutex_list->t);
// 	}

// 	if (remaining_time == 0) {
// 		pthread_mutex_lock(set->mutex_list->p);
// 		printf("%ds : %d finished\n", set->values->time, running_id);
// 		pthread_mutex_unlock(set->mutex_list->p);
// 		pthread_mutex_lock(set->mutex_list->cpu);
// 		set->values->process_on_cpu = -1;
// 		set->values->cpu_working = FALSE;
// 		pthread_mutex_unlock(set->mutex_list->cpu);
// 		pthread_mutex_lock(set->mutex_list->t);
// 		set->values->time = set->values->time + CONTEXT_SWITCH;
// 		pthread_mutex_unlock(set->mutex_list->t);
// 		pthread_mutex_lock(set->mutex_list->r_t);
// 		set->values->remaining_time = -1;
// 		pthread_mutex_unlock(set->mutex_list->r_t);
// 		set->counter++;

// 		free(job);
// 		job = NULL;

// 	} else {
// 		pthread_mutex_lock(set->mutex_list->cpu);
// 		set->values->cpu_working = TRUE;
// 		pthread_mutex_unlock(set->mutex_list->cpu);
// 		pthread_mutex_lock(set->mutex_list->t);
// 		set->values->time++;
// 		set->values->loaded_process_execution_time++;
// 		pthread_mutex_unlock(set->mutex_list->t);
// 		pthread_mutex_lock(set->mutex_list->r_t);
// 		set->values->remaining_time = -1;
// 		pthread_mutex_unlock(set->mutex_list->r_t);
// 	}
// }

// void *round_robin(void *arg) {
// 	t_setting *set;
// 	t_ready_queue *ready_queue;
// 	t_ready_queue *job;
// 	int running_id = -1;
// 	int execution_time = 0;

// 	set = (t_setting *)arg;
// 	ready_queue = NULL;
// 	job = NULL;

// 	wait_starting(set);

// 	printf("round_robin start\n");
// 	while (1) {
// 		if (set->values->remain_thread_count == 0 || set->counter == set->total_process_count) {
// 			break;
// 		}
// 		wait_routine(set);
// 		usleep(100);

// 		pthread_mutex_lock(set->mutex_list->cpu);
// 		running_id = set->values->process_on_cpu;
// 		pthread_mutex_unlock(set->mutex_list->cpu);
// 		pthread_mutex_lock(set->mutex_list->t);
// 		execution_time = set->values->loaded_process_execution_time;
// 		pthread_mutex_unlock(set->mutex_list->t);
// 		if (job != NULL)
// 			printf("job id %d\n", job->id);
// 		else {
// 			printf("job is NULL\n");
// 			printf("ready_queue->next->id : %d\n", ready_queue->next->id);
// 		}
// 		if (execution_time == QUANTUM || running_id == -1) {
// 			job_one(set, ready_queue, job);
// 			pthread_mutex_lock(set->mutex_list->t);
// 			set->values->loaded_process_execution_time = 0;
// 			pthread_mutex_unlock(set->mutex_list->t);
// 		} else {
// 			job_two_c(set, running_id, job);
// 		}
// 		exit_routine(set);
// 	}
// 	pthread_mutex_lock(set->mutex_list->p);
// 	printf("round_robin end\n");
// 	pthread_mutex_unlock(set->mutex_list->p);

// 	return (0);
// }

static void job_one(t_setting *set, t_ready_queue **ready_queue, t_ready_queue **job, int time) {
    t_ready_queue *temp;
    t_ready_queue *last;
    t_ready_queue *c_job;
    int id;

    pthread_mutex_lock(set->mutex_list->ready_queue);
    *ready_queue = set->values->ready_queue;
    c_job = *job;

	last = *ready_queue;
    while (last->next != NULL) {
        last = last->next;
	}
    last->next = c_job;

    if ((*ready_queue)->next != NULL) {
        pthread_mutex_lock(set->mutex_list->p);
		printf("%ds : Monitor : ready queue list : ", time);
        for (t_ready_queue *temp = (*ready_queue)->next; temp != NULL; temp = temp->next) {
            printf("%d, ", temp->id);
        }
        printf("\n");
		pthread_mutex_unlock(set->mutex_list->p);

        *job = (*ready_queue)->next;
        temp = *job;

        (*ready_queue)->next = temp->next;
        temp->next = NULL;


        id = (*job)->id;

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
            return ;
        }
        printf("%ds : Monitor : no process\n", set->values->time);
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
        printf("%ds : Monitor : %d finished\n", set->values->time, running_id);
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
		printf("%ds : Monitor : %d is working\n", time, running_id);
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

void *round_robin(void *arg) {
    t_setting *set;
    t_ready_queue *ready_queue;
    t_ready_queue *job;
    int remain_thread_count;
    int running_id = -2;
    int execution_time = 0;
	int time = 0;

    set = (t_setting *)arg;
    ready_queue = NULL;
    job = NULL;
	time = 0;

    wait_starting(set);

    printf("round_robin start\n");
    while (1) {
        pthread_mutex_lock(set->mutex_list->check);
        remain_thread_count = set->values->remain_thread_count;
        pthread_mutex_unlock(set->mutex_list->check);
        if (remain_thread_count == 0 || set->counter == set->total_process_count) {
            break;
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
            job_one(set, &ready_queue, &job, time);
            pthread_mutex_lock(set->mutex_list->t);
            set->values->loaded_process_execution_time = 0;
            pthread_mutex_unlock(set->mutex_list->t);
        } else {
            job_two_c(set, running_id, &job, time);
        }
        exit_routine(set);
    }
    pthread_mutex_lock(set->mutex_list->p);
    printf("round_robin end\n");
    pthread_mutex_unlock(set->mutex_list->p);

    return (0);
}
