#include "init.h"
#include "scheduling.h"
#include <unistd.h>

int main(int argc, char **argv) {
	t_setting *setting;
	t_process *processes;
	int mode;

	setting = init_setting(argv, processes, &mode);
	if (setting == NULL) {
		write(2, "Error: Setting initialization failed\n", 36);
		return 1;
	}

	scheduler(setting, processes, mode);

	join_threads(setting, processes);

	// memory_free(setting);

	return 0;
}