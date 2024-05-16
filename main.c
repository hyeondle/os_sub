#include "init.h"
#include "scheduling.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

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
	printf("test %d\n", processes->burst_time);
	scheduler(setting, processes, mode);
	join_threads(setting, processes);

	memory_free(setting, processes);

	// system("leaks a.out");

	return 0;
}