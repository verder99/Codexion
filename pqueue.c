/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   pqueue.c                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: aslobodi <aslobodi@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2026/08/21 16:46:44 by aslobodi      #+#    #+#                 */
/*   Updated: 2026/08/21 17:06:18 by aslobodi      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	first_one_is_smaller(t_request *new, t_request *old)
{
	if (new->ts != old->ts)
		return (new->ts < old->ts);
	if (new->request_time != old->request_time)
		return (new->request_time < old->request_time);
	return (new->coder_id < old->coder_id);
}

void	pqueue_push(t_dongle *dongle, t_request request)
{
	if (dongle->queue.size == 0)
	{
		dongle->queue.size = 1;
		dongle->queue.requests[0] = request;
	}
	else if (dongle->queue.size == 1)
	{
		if (first_one_is_smaller(&request, &dongle->queue.requests[0]))
		{
			dongle->queue.requests[1] = dongle->queue.requests[0];
			dongle->queue.requests[0] = request;
		}
		else
			dongle->queue.requests[1] = request;
		dongle->queue.size = 2;
	}
}

void	pqueue_pop(t_dongle *dongle)
{
	if (dongle->queue.size == 1)
		dongle->queue.size = 0;
	else if (dongle->queue.size == 2)
	{
		dongle->queue.requests[0] = dongle->queue.requests[1];
		dongle->queue.size = 1;
	}
}

int	pqueue_peek(t_dongle *dongle)
{
	if (dongle->queue.size == 0)
		return (-1);
	return (dongle->queue.requests[0].coder_id);
}
