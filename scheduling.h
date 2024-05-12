#ifndef SCHEDULING_H
# define SCHEDULING_H

# include "init.h"

typedef enum s_state {
	ARRIVED,
	LOADED,
	UNLOADED,
	FINISHED
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

// cycle.c
void	*cycle(void *arg);
// fcfs.c
void	*fcfs(void *arg);
// sjf.c
void	*sjf(void *arg);
// round_robin.c
void	*round_robin(void *arg);
// psjf.c
void	*psjf(void *arg);

#endif