#include "init.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

t_process *new_process(char *buffer, t_setting *setting) {
	t_process *process;
	char **tokens;

	setting->total_process_count++;

	process = (t_process *)malloc(sizeof(t_process));
	if (process == NULL) {
		write(2, "Error: Process initialization failed\n", 37);
		exit(1);
	}

	tokens = ft_split(buffer, ' ');

	process->id = atoi(tokens[0]);
	process->burst_time = atoi(tokens[1]);
	process->arrival_time = atoi(tokens[2]);
	process->priority = atoi(tokens[3]);
	process->waiting_time = 0;
	process->remaining_time = process->burst_time;
	process->turnaround_time = 0;
	process->response_time = -1;
	process->values = setting->values;
	process->submitted = FALSE;
	process->loaded = FALSE;
	process->next = NULL;
	process->mutex_list = setting->mutex_list;

	if (setting->maximum_arrival_time < process->arrival_time)
		setting->maximum_arrival_time = process->arrival_time;

	for (int i = 0; tokens[i] != NULL; i++)
		free(tokens[i]);
	free(tokens);

	return (process);
}

t_mutex_list *create_mutex_list() {
	t_mutex_list *mutex_list;
	int i, j, k, l, m, n;


	mutex_list = (t_mutex_list *)malloc(sizeof(t_mutex_list));
	if (mutex_list == NULL) {
		write(2, "Error: Mutex list initialization failed\n", 41);
		exit(1);
	}

	mutex_list->cpu = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	mutex_list->t = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	mutex_list->ready_queue = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	mutex_list->p = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	mutex_list->check = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	mutex_list->r_t = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	if (mutex_list->cpu == NULL || mutex_list->t == NULL || mutex_list->ready_queue == NULL || mutex_list->p == NULL || mutex_list->check == NULL || mutex_list->r_t == NULL) {
		write(2, "Error: Mutex initialization failed\n", 36);
		exit(1);
	}

	i = pthread_mutex_init(mutex_list->cpu, NULL);
	j = pthread_mutex_init(mutex_list->t, NULL);
	k = pthread_mutex_init(mutex_list->ready_queue, NULL);
	l = pthread_mutex_init(mutex_list->p, NULL);
	m = pthread_mutex_init(mutex_list->check, NULL);
	n = pthread_mutex_init(mutex_list->r_t, NULL);

	if (i != 0 || j != 0 || k != 0 || l != 0 || m != 0 || n != 0) {
		write(2, "Error: Mutex initialization failed\n", 36);
		exit(1);
	}

	return (mutex_list);
}

t_ready_queue	*make_ready_queue(int id) {
	t_ready_queue *queue;

	queue = (t_ready_queue *)malloc(sizeof(t_ready_queue));
	if (queue == NULL) {
		write(2, "Error: Ready queue initialization failed\n", 41);
		exit(1);
	}

	queue->id = id;
	queue->next = NULL;

	return queue;
}

t_setting	*init_setting(char **argv, int *mode) {
	int fd;
	char *buffer;
	t_setting *setting;
	t_process *proc_list;
	t_process *temp;
	t_mutex_list *mutex_list;
	t_values *values;

	setting = (t_setting *)malloc(sizeof(t_setting));
	if (setting == NULL)
		return NULL;

	buffer = (char *)malloc(1024 * sizeof(char));
	if (buffer == NULL) {
		free(setting);
		return NULL;
	}
	memset(buffer, 0, 1024 * sizeof(char));

	proc_list = (t_process *)malloc(sizeof(t_process));
	if (proc_list == NULL) {
		free(setting);
		free(buffer);
		return NULL;
	}

	*mode = atoi(argv[2]);

	mutex_list = create_mutex_list();

	values = (t_values *)malloc(sizeof(t_values));
	if (values == NULL) {
		free(setting);
		free(buffer);
		free(proc_list);
		return NULL;
	}

	values->time = 0;
	values->thread_count = 0;
	values->loaded_process_execution_time = 0;
	values->process_on_cpu = -1;
	values->priority = -1;
	values->ready_queue = make_ready_queue(0);
	values->checked_count = 0;
	values->checked_count2 = 0;
	values->routine = FALSE;
	values->remaining_time = -1;
	values->cpu_working = FALSE;

	setting->maximum_arrival_time = 0;
	setting->total_process_count = 0;
	setting->values = values;
	setting->mutex_list = mutex_list;

	proc_list->id = 0;
	proc_list->burst_time = 0;
	proc_list->arrival_time = 0;
	proc_list->priority = 0;
	proc_list->waiting_time = 0;
	proc_list->remaining_time = 0;
	proc_list->turnaround_time = 0;
	proc_list->response_time = 0;
	proc_list->submitted = FALSE;
	proc_list->loaded = FALSE;
	proc_list->mutex_list = mutex_list;
	proc_list->values = values;
	proc_list->next = NULL;

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		free(setting);
		free(buffer);
		free(proc_list);
		return NULL;
	}

	temp = proc_list;

	int loop_depth = 0;
	while (loop_depth < 1024 && read(fd, buffer + strlen(buffer), 1) > 0) {
		if (buffer[strlen(buffer) - 1] == '\n') {
			if (buffer[0] != '#' && buffer[0] != '\0') {
				temp->next = new_process(buffer, setting);
				temp = temp->next;
			}
			memset(buffer, 0, 1024 * sizeof(char));
			buffer[0] = '\0';
			loop_depth = 0;
		}
		loop_depth++;
		buffer[loop_depth + 1] = '\0';
	}
	if (buffer[0] != '#' && buffer[0] != '\0')
		temp->next = new_process(buffer, setting);
	free(buffer);
	close(fd);

	values->thread_count = setting->total_process_count;
	values->remain_thread_count = setting->total_process_count;

	setting->processes = proc_list;

	return setting;
}