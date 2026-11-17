/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   codexion.h                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: aslobodi <aslobodi@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2026/08/06 15:58:46 by aslobodi      #+#    #+#                 */
/*   Updated: 2026/08/21 17:05:37 by aslobodi      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H
# include <pthread.h>
# include <string.h>
# include <stdio.h>
# include <time.h>
# include <sys/time.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>

typedef struct s_state	t_state;
typedef struct s_dongle	t_dongle;

typedef struct s_arguments
{
	int		number_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		number_of_compiles_required;
	int		dongle_cooldown;
	bool	is_fifo;
}	t_arguments;

typedef struct s_coder
{
	int			id;
	int			status;
	int			compiles_ammount;
	long long	ts_last_compile;
	t_state		*state;
	pthread_t	th;
	t_dongle	*left;
	t_dongle	*right;
}	t_coder;

typedef struct s_request
{
	int			coder_id;
	long long	ts;
	long long	request_time;
}	t_request;

typedef struct s_heapq
{
	t_request	requests[2];
	int			size;
}	t_heapq;

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	long long		last_released;
	bool			is_in_use;
	t_heapq			queue;
}	t_dongle;

typedef struct s_state
{
	pthread_t		monitor;
	long long		tv_start;
	bool			is_finished;
	t_arguments		args;
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_mutex_t	mutex_print;
	pthread_mutex_t	mutex_state;
}	t_state;

int			parse_args(int argc, char *argv[], t_arguments *args);

int			init_all(t_arguments *args, t_state *state);

void		*monitor_routine(void *arg);

bool		take_dongle(t_dongle *dongle, t_coder *coder);
void		drop_dongle(t_dongle *dongle);

void		pqueue_push(t_dongle *dongle, t_request request);
void		pqueue_pop(t_dongle *dongle);
int			pqueue_peek(t_dongle *dongle);

long long	get_ts_in_ms(void);
bool		check_end(t_state *state);
void		print_log(t_coder *coder, char *status);
int			init_clean(t_state *state, int d_count);

#endif