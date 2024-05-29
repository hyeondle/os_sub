#ifndef SCHEDULING_H
# define SCHEDULING_H

# include "init.h"
#include <sys/wait.h>

typedef enum s_state {
	ARRIVED,
	LOADED,
	STARTED,
	FINISHED,
	READY,
	WAITING,
	RUNNING,
	TERMINATED
} t_state;

typedef struct s_fcfs {
	int time;
	int counter;
	int running;
	int done;
	int total_waiting_time;
	int total_turnaround_time;
	float avg_waiting_time;
	float avg_turnaround_time;
}	t_fcfs;

void scheduler(t_setting *setting, t_process *processes, int mode);
void join_threads(t_setting *setting, t_process *processes);
void printer(t_process *p, t_state state, int time);

void job_two(t_setting *set, int running_id);

// monitoring thread's f
void wait_starting(t_setting *set);
void wait_routine(t_setting *set);
void exit_routine(t_setting *set);

// cycle.c
void	*cycle(void *arg);
// fcfs.c
void	*fcfs(void *arg);
// sjf.c
void	*sjf(void *arg);
// round_robin.c
void	*round_robin(void *arg);
// srtf.c
void	*srtf(void *arg);
// lrrwp.c
void	*lrrwp(void *arg);
#endif