#include "init.h"
#include "scheduling.h"
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {
	t_setting *setting;
	t_process *processes;
	int mode;

	setting = init_setting(argv, processes, &mode);
	if (setting == NULL) {
		write(2, "Error: Setting initialization failed\n", 36);
		return 1;
	}
	printf("setting initialized\n");
	printf("%d\n", setting->values->thread_count);
	printf("test\n");
	scheduler(setting, processes, mode);
	printf("scheduler started\n");
	join_threads(setting, processes);
	printf("threads joined\n");
	// memory_free(setting);

	return 0;
}