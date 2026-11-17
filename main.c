/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: aslobodi <aslobodi@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2026/06/18 20:20:51 by aslobodi      #+#    #+#                 */
/*   Updated: 2026/08/21 18:34:04 by aslobodi      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	get_both_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->left->id < coder->right->id)
	{
		first = coder->left;
		second = coder->right;
	}
	else
	{
		first = coder->right;
		second = coder->left;
	}
	if (!take_dongle(first, coder))
		return (false);
	if (!take_dongle(second, coder))
	{
		drop_dongle(first);
		return (false);
	}
	return (true);
}

void	*routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->id % 2 == 0)
		usleep(50000);
	while (!check_end(coder->state))
	{
		if (!get_both_dongles(coder))
			return (NULL);
		pthread_mutex_lock(&coder->state->mutex_state);
		coder->ts_last_compile = get_ts_in_ms();
		pthread_mutex_unlock(&coder->state->mutex_state);
		print_log(coder, "is compiling");
		usleep(coder->state->args.time_to_compile * 1000);
		pthread_mutex_lock(&coder->state->mutex_state);
		coder->compiles_ammount += 1;
		pthread_mutex_unlock(&coder->state->mutex_state);
		drop_dongle(coder->left);
		drop_dongle(coder->right);
		print_log(coder, "is debugging");
		usleep(coder->state->args.time_to_debug * 1000);
		print_log(coder, "is refactoring");
		usleep(coder->state->args.time_to_refactor * 1000);
	}
	return (NULL);
}

int	abort_threads(t_state *state, int n)
{
	int	i;

	pthread_mutex_lock(&state->mutex_state);
	state->is_finished = true;
	pthread_mutex_unlock(&state->mutex_state);
	i = 0;
	while (i < state->args.number_of_coders)
	{
		pthread_mutex_lock(&state->dongles[i].mutex);
		pthread_cond_broadcast(&state->dongles[i].cond);
		pthread_mutex_unlock(&state->dongles[i].mutex);
		i++;
	}
	i = 0;
	while (i < n)
	{
		pthread_join(state->coders[i].th, NULL);
		i++;
	}
	return (1);
}

int	run_sim(t_arguments *args, t_state *state)
{
	int	i;
	int	n;

	i = 0;
	n = args->number_of_coders;
	while (i < n)
	{
		if (pthread_create(&state->coders[i].th, NULL,
				&routine, &state->coders[i]) != 0)
			return (abort_threads(state, i));
		i++;
	}
	if (pthread_create(&state->monitor, NULL, &monitor_routine, state) != 0)
		return (abort_threads(state, i));
	i = 0;
	while (i < n)
	{
		pthread_join(state->coders[i].th, NULL);
		i++;
	}
	pthread_join(state->monitor, NULL);
	return (0);
}

int	main(int argc, char *argv[])
{
	t_arguments	args;
	t_state		state;

	if (parse_args(argc, argv, &args))
	{
		fprintf(stderr, "Error!\n");
		return (1);
	}
	if (init_all(&args, &state))
		return (1);
	run_sim(&args, &state);
	init_clean(&state, args.number_of_coders);
	return (0);
}
