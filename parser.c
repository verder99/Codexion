/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parser.c                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: aslobodi <aslobodi@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2026/08/15 13:14:40 by aslobodi      #+#    #+#                 */
/*   Updated: 2026/08/21 16:42:52 by aslobodi      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	ft_atoi_strict(const char *nptr, int *out_val)
{
	int		i;
	long	result;

	if (!nptr || nptr[0] == '\0')
		return (false);
	i = 0;
	result = 0;
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		result = result * 10 + (nptr[i] - '0');
		if (result > 2147483647)
			return (false);
		i++;
	}
	if (nptr[i] != '\0')
		return (false);
	*out_val = (int)result;
	return (true);
}

int	parse_args(int argc, char *argv[], t_arguments *args)
{
	if (argc != 9 || !args)
		return (1);
	if (!ft_atoi_strict(argv[1], &args->number_of_coders)
		|| !ft_atoi_strict(argv[2], &args->time_to_burnout)
		|| !ft_atoi_strict(argv[3], &args->time_to_compile)
		|| !ft_atoi_strict(argv[4], &args->time_to_debug)
		|| !ft_atoi_strict(argv[5], &args->time_to_refactor)
		|| !ft_atoi_strict(argv[6], &args->number_of_compiles_required)
		|| !ft_atoi_strict(argv[7], &args->dongle_cooldown))
		return (1);
	if (args->number_of_coders <= 0 || args->time_to_burnout <= 0
		|| args->time_to_compile <= 0 || args->time_to_debug <= 0
		|| args->time_to_refactor <= 0
		|| args->number_of_compiles_required <= 0 || args->dongle_cooldown < 0)
		return (1);
	if (strcmp(argv[8], "fifo") == 0)
		args->is_fifo = true;
	else if (strcmp(argv[8], "edf") == 0)
		args->is_fifo = false;
	else
		return (1);
	return (0);
}
