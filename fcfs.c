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

/*
수정 방법 :
remaining_time은 -1인 상태이다.
cpu에 프로세스가 로드되면 해당 시간은 0으로 변경된다.
그러면 해당 시간은 프로세스 스레드에 의해 해당 프로세스의 burst_time으로 변경된다.
모니터링 스레드는 해당 시간이 0이 아님을 인지하면 해당 프로세스의 remaining_time을 1 감소시킨다.
기존 스레드는 해당 시간이 바뀌었음을 인지하면(1 감소를 인지하면) 자신의 remaining_time을 해당 시간으로 바꾼다.
그리고 해당 시간이 0이 되면 해당 프로세스의 remaining_time을 -1로 바꾼다.
그러면 모니터링 스레드는 해당 프로세스의 remaining_time이 -1임을 인지하고 해당 프로세스의 스레드를 종료시킨다.
*/

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
			printf("ready_queue list : \n");
			printf("id : ");
			for (temp = ready_queue->next; temp != NULL; temp = temp->next) {
				printf("%d, ", temp->id);
			}
			printf("\n");
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
			printf("%ds : no process\n", set->values->time);
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
		printf("%ds : %d started\n", set->values->time, id);
		pthread_mutex_unlock(set->mutex_list->cpu);


		//print
		pthread_mutex_lock(set->mutex_list->p);
		printf("%ds : %d loaded on cpu\n", set->values->time, id);
		pthread_mutex_unlock(set->mutex_list->p);

		pthread_mutex_lock(set->mutex_list->t);
		set->values->time = set->values->time + burst_time;
		pthread_mutex_unlock(set->mutex_list->t);


		pthread_mutex_lock(set->mutex_list->cpu);
		set->values->process_on_cpu = -1;
		pthread_mutex_unlock(set->mutex_list->cpu);

		//print
		pthread_mutex_lock(set->mutex_list->p);
		printf("%ds : %d finished\n", set->values->time, id);
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