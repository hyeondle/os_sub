#include "scheduling.h"
#include "init.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static void create_scheduler(t_setting *setting, int mode) {
	if (mode == 1) {
		pthread_create(&(setting->thread_id), NULL, fcfs, setting);
	}
	// } else if (mode == 2) {
	// 	pthread_create(&(setting->thread_id), NULL, sjf, setting);
	// } else if (mode == 3) {
	// 	pthread_create(&(setting->thread_id), NULL, round_robin, setting);
	// } else if (mode == 4) {
	// 	pthread_create(&(setting->thread_id), NULL, psjf, setting);
	// }
}

void scheduler(t_setting *setting, t_process *processes, int mode) {
	t_process *process;
	create_scheduler(setting, mode);
	process = processes->next;
	printf("process count: %d\n", setting->total_process_count);
	for (int i = 0; i < setting->total_process_count; i++) {
		if (process == NULL) {
			fprintf(stderr, "Error: Process not found\n");
			exit(1);
		}
		if (pthread_create(&(process->thread_id), NULL, cycle, (void *)process) != 0) {
			fprintf(stderr, "Error: Thread creation failed\n");
			exit(1);
		}
		process = process->next;
	}
}

void join_threads(t_setting *setting, t_process *processes) {
	t_process *process;
	process = processes->next;
	printf("total:count %d\n", setting->total_process_count);
	for (int i = 0; i < setting->total_process_count; i++) {
		pthread_join(process->thread_id, NULL);
		process = process->next;
	}
	pthread_join(setting->thread_id, NULL);
}

void	wait_starting(t_setting *set) {
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

void	wait_routine(t_setting *set) {
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

void	exit_routine(t_setting *set) {
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
