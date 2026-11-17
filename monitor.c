/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   monitor.c                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: aslobodi <aslobodi@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2026/08/12 18:30:09 by aslobodi      #+#    #+#                 */
/*   Updated: 2026/08/21 16:46:09 by aslobodi      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	*inform_all(t_state *state)
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
	return (NULL);
}

// 0 - no burnout and not enought compiles
// 1 - no burnout and enought compiles
// 2 - burnout
static int	check_coder(t_coder *coder)
{
	long long	dif;
	int			comp_req;

	comp_req = coder->state->args.number_of_compiles_required;
	pthread_mutex_lock(&coder->state->mutex_state);
	if (coder->compiles_ammount >= comp_req)
	{
		pthread_mutex_unlock(&coder->state->mutex_state);
		return (1);
	}
	dif = get_ts_in_ms() - coder->ts_last_compile;
	if (dif > coder->state->args.time_to_burnout)
	{
		pthread_mutex_unlock(&coder->state->mutex_state);
		print_log(coder, "burned out");
		return (2);
	}
	pthread_mutex_unlock(&coder->state->mutex_state);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_state	*state;
	int		i;
	int		n;
	int		status;

	state = (t_state *)arg;
	while (!check_end(state))
	{
		i = 0;
		n = 0;
		while (i < state->args.number_of_coders)
		{
			status = check_coder(&state->coders[i]);
			if (status == 2)
				return (inform_all(state));
			else if (status == 1)
				n += 1;
			i++;
		}
		if (n == state->args.number_of_coders)
			return (inform_all(state));
		usleep(1000);
	}
	return (NULL);
}
