#include "init.h"
#include "scheduling.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void print_status(t_setting *set, t_process *process) {
	t_process *temp;
	float avg_waiting_time = 0;
	float avg_turnaround_time = 0;
	float avg_response_time = 0;

	printf("\nResult\n");
	temp = process->next;
	while (temp != NULL) {
		avg_response_time += temp->response_time;
		avg_waiting_time += temp->waiting_time;
		avg_turnaround_time += temp->turnaround_time;
		temp = temp->next;
	}
	avg_response_time /= set->total_process_count;
	avg_waiting_time /= set->total_process_count;
	avg_turnaround_time /= set->total_process_count;
	printf("Execution Time: %d\n\n", set->values->time);
	printf("Average Response Time: %.2f\n", avg_response_time);
	printf("Average Waiting Time: %.2f\n", avg_waiting_time);
	printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
	printf("\n");
}

int main(int argc, char **argv) {
	t_setting *setting;
	t_process *processes;
	int mode;

	if (argc != 3) {
		write(2, "Error: Invalid arguments\n", 26);
		return 1;
	}
	setting = init_setting(argv, &mode);
	processes = setting->processes;
	if (setting == NULL) {
		write(2, "Error: Setting initialization failed\n", 36);
		return 1;
	}
	printf("%d thread will be created\n", setting->values->thread_count);
	scheduler(setting, processes, mode);
	join_threads(setting, processes);
	print_status(setting, processes);
	memory_free(setting, processes);

	return 0;
}