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
		printf("create thread %d\n", process->id);
		if (pthread_create(&(process->thread_id), NULL, cycle, (void *)process) != 0) {
			fprintf(stderr, "Error: Thread creation failed\n");
			exit(1);
		}
		// pthread_mutex_lock(process->mutex_list->cpu);
		// process->values->thread_count--;
		// pthread_mutex_unlock(process->mutex_list->cpu);
		process = process->next;
	}
}

void join_threads(t_setting *setting, t_process *processes) {
	t_process *process;
	process = processes->next;
	for (int i = 0; i < setting->total_process_count; i++) {
		if (process == NULL) {
			fprintf(stderr, "Error: Process not found\n");
			exit(1);
		}
		if (pthread_join(process->thread_id, NULL) != 0) {
			fprintf(stderr, "Error: Thread join failed\n");
			exit(1);
		}
		process = process->next;
	}
	if (pthread_join(setting->thread_id, NULL) != 0) {
		fprintf(stderr, "Error: Thread join failed\n");
		exit(1);
	}
}

void printer(t_process *p, t_state state, int time) {
	int id;

	id = p->id;

	pthread_mutex_lock(p->mutex_list->p);
	if (state == ARRIVED) {
		printf("%d : %d arrived\n", time, id);
	} else if (state == LOADED) {
		printf("%d : %d loaded on cpu\n", time, id);
	} else if (state == STARTED) {
		printf("%d : %d started\n", time, id);
	} else if (state == FINISHED) {
		printf("%d : %d finished\n", time, id);
	} else if (state == READY) {
		printf("%d : %d ready\n", time, id);
	} else if (state == WAITING) {
		printf("%d : %d waiting\n", time, id);
	} else if (state == RUNNING) {
		printf("%d : %d running\n", time, id);
	} else if (state == TERMINATED) {
		printf("%d : %d terminated\n", time, id);
	}
}