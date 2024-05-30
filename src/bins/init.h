#ifndef STRUCTS_H
#define STRUCTS_H

# include <pthread.h>

# ifndef CONTEXT_SWITCH
#  define CONTEXT_SWITCH 0
# endif

# ifndef QUANTUM
#  define QUANTUM 20
# endif

# ifndef RQ_SIZE
#  define RQ_SIZE 3
# endif

typedef enum e_bool {
	FALSE,
	TRUE
}	t_bool;


typedef struct s_ready_queue {
	int		id;
	int 	priority;
	int		burst_time;
	int		arrival_time;
	int		remaining_time;
	struct s_ready_queue	*next;
}	t_ready_queue;


typedef struct s_values {
	int time;
	int thread_count;
	int remain_thread_count;
	int checked_count;
	int checked_count2;
	int loaded_process_execution_time;
	int process_on_cpu;
	int priority;
	int remaining_time;
	t_bool routine;
	t_bool cpu_working;
	struct s_ready_queue *ready_queue;
}	t_values;

typedef struct s_mutex_list {
	pthread_mutex_t	*cpu;
	pthread_mutex_t	*t;
	pthread_mutex_t *p;
	pthread_mutex_t *ready_queue;
	pthread_mutex_t *check;
	pthread_mutex_t *r_t;
}	t_mutex_list;

typedef struct s_process {
	//inputs
	int		id;
	int		burst_time;
	int		arrival_time;
	int		priority;
	//calculating
	int waiting_time;
	int remaining_time;
	int turnaround_time;
	int response_time;

	t_values *values;

	t_bool	submitted;
	t_bool	loaded;
	pthread_t		thread_id;
	t_mutex_list	*mutex_list;

	struct s_process	*next;
}	t_process;

typedef struct s_setting {
	int	time;
	int	counter;
	int process_on_cpu;

	int total_process_count;
	int maximum_arrival_time;

	t_process	*processes;

	t_values *values;

	t_mutex_list	*mutex_list;
	pthread_t		thread_id;
}	t_setting;

t_setting	*init_setting(char **argv, int *mode);
void memory_free(t_setting *setting, t_process *processes);

#endif