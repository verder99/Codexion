/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   init.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: aslobodi <aslobodi@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2026/08/15 13:15:55 by aslobodi      #+#    #+#                 */
/*   Updated: 2026/08/21 16:35:28 by aslobodi      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_clean(t_state *state, int d_count)
{
	int	i;

	i = 0;
	while (i < d_count)
	{
		pthread_mutex_destroy(&state->dongles[i].mutex);
		pthread_cond_destroy(&state->dongles[i].cond);
		i++;
	}
	pthread_mutex_destroy(&state->mutex_print);
	pthread_mutex_destroy(&state->mutex_state);
	if (state->dongles)
		free(state->dongles);
	if (state->coders)
		free(state->coders);
	return (1);
}

static int	init_dongles(t_state *state)
{
	int	n;
	int	i;

	n = state->args.number_of_coders;
	i = 0;
	while (i < n)
	{
		state->dongles[i].id = i + 1;
		state->dongles[i].last_released = 0;
		state->dongles[i].is_in_use = false;
		state->dongles[i].queue.size = 0;
		if (pthread_mutex_init(&state->dongles[i].mutex, NULL) != 0)
			return (init_clean(state, i));
		if (pthread_cond_init(&state->dongles[i].cond, NULL) != 0)
		{
			pthread_mutex_destroy(&state->dongles[i].mutex);
			return (init_clean(state, i));
		}
		i++;
	}
	return (0);
}

static int	init_coders(t_state *state)
{
	int	n;
	int	i;

	n = state->args.number_of_coders;
	i = 0;
	while (i < n)
	{
		state->coders[i].id = i + 1;
		state->coders[i].compiles_ammount = 0;
		state->coders[i].status = 0;
		state->coders[i].ts_last_compile = get_ts_in_ms();
		state->coders[i].left = &state->dongles[i];
		state->coders[i].right = &state->dongles[(i + 1) % n];
		state->coders[i].state = state;
		i++;
	}
	return (0);
}

int	init_all(t_arguments *args, t_state *state)
{
	int	n;

	n = args->number_of_coders;
	state->args = *args;
	state->is_finished = false;
	state->tv_start = get_ts_in_ms();
	if (pthread_mutex_init(&state->mutex_print, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&state->mutex_state, NULL) != 0)
	{
		pthread_mutex_destroy(&state->mutex_print);
		return (1);
	}
	state->coders = NULL;
	state->dongles = NULL;
	state->coders = malloc(sizeof(t_coder) * n);
	if (!state->coders)
		return (init_clean(state, 0));
	state->dongles = malloc(sizeof(t_dongle) * n);
	if (!state->dongles)
		return (init_clean(state, 0));
	if (init_dongles(state) || init_coders(state))
		return (1);
	return (0);
}
