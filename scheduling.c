#include "scheduling.h"
#include "init.h"
#include <stdio.h>

static void create_scheduler(t_setting *setting, int mode) {
	if (mode == 1) {
		pthread_create(&(setting->thread_id), NULL, fcfs, setting);
	} else if (mode == 2) {
		pthread_create(&(setting->thread_id), NULL, sjf, setting);
	} else if (mode == 3) {
		pthread_create(&(setting->thread_id), NULL, round_robin, setting);
	} else if (mode == 4) {
		pthread_create(&(setting->thread_id), NULL, psjf, setting);
	}
}

void scheduler(t_setting *setting, t_process *processes, int mode) {
	t_process *process;
	// 모니터링 스레드에서 전체 로딩까지 기다리는 것을 추가할지 말지 고려할 것
	create_scheduler(setting, mode);
	process = processes->next;
	for (int i = 0; i < setting->total_process_count; i++) {
		pthread_create(&(process->thread_id), NULL, cycle, &process);
		process = process->next;
	}
}

void join_threads(t_setting *setting, t_process *processes) {
	t_process *process;
	process = processes->next;
	for (int i = 0; i < setting->total_process_count; i++) {
		pthread_join(process->thread_id, NULL);
		process = process->next;
	}
	pthread_join(setting->thread_id, NULL);
}

void printer(int id, int time, t_state state) {

	pthread_mutex_lock(p->mutex_list->p);
	if (state == ARRIVED) {
		printf("%d : %d arrived\n", time, id);
	} else if (state == LOADED) {
		printf("%d : %d loaded on cpu\n", time, id);
	}
}