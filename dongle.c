/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   dongle.c                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: aslobodi <aslobodi@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2026/08/15 17:54:41 by aslobodi      #+#    #+#                 */
/*   Updated: 2026/08/21 17:06:04 by aslobodi      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	drop_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_in_use = false;
	dongle->last_released = get_ts_in_ms();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

static t_request	make_request(t_coder *coder)
{
	t_request	request;

	request.coder_id = coder->id;
	request.request_time = get_ts_in_ms();
	if (coder->state->args.is_fifo)
		request.ts = request.request_time;
	else
	{
		pthread_mutex_lock(&coder->state->mutex_state);
		request.ts = coder->ts_last_compile;
		pthread_mutex_unlock(&coder->state->mutex_state);
	}
	return (request);
}

static bool	wait_dongle_ready(t_dongle *dongle, t_coder *coder)
{
	long long	dur;

	while (!check_end(coder->state))
	{
		while (dongle->is_in_use || pqueue_peek(dongle) != coder->id)
		{
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
			if (check_end(coder->state))
				return (false);
		}
		dur = get_ts_in_ms() - dongle->last_released;
		if (dur < coder->state->args.dongle_cooldown)
		{
			pthread_mutex_unlock(&dongle->mutex);
			usleep((coder->state->args.dongle_cooldown - dur) * 1000);
			pthread_mutex_lock(&dongle->mutex);
			continue ;
		}
		return (true);
	}
	return (false);
}

bool	take_dongle(t_dongle *dongle, t_coder *coder)
{
	pthread_mutex_lock(&dongle->mutex);
	pqueue_push(dongle, make_request(coder));
	if (!wait_dongle_ready(dongle, coder))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (false);
	}
	pqueue_pop(dongle);
	dongle->is_in_use = true;
	print_log(coder, "has taken a dongle");
	pthread_mutex_unlock(&dongle->mutex);
	return (true);
}
