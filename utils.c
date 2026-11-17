/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   utils.c                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: aslobodi <aslobodi@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2026/08/15 13:13:48 by aslobodi      #+#    #+#                 */
/*   Updated: 2026/08/21 16:40:20 by aslobodi      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_ts_in_ms(void)
{
	struct timeval	tv;
	long long		result;

	gettimeofday(&tv, NULL);
	result = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return (result);
}

bool	check_end(t_state *state)
{
	bool	is_finished;

	pthread_mutex_lock(&state->mutex_state);
	is_finished = state->is_finished;
	pthread_mutex_unlock(&state->mutex_state);
	return (is_finished);
}

void	print_log(t_coder *coder, char *status)
{
	long long	timestamp;

	pthread_mutex_lock(&coder->state->mutex_print);
	if (!check_end(coder->state))
	{
		timestamp = get_ts_in_ms() - coder->state->tv_start;
		printf("%lld %d %s\n", timestamp, coder->id, status);
	}
	pthread_mutex_unlock(&coder->state->mutex_print);
}
