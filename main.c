/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdait-s <abdait-s@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 11:49:31 by abdait-s          #+#    #+#             */
/*   Updated: 2026/08/26 11:11:15 by abdait-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* every argument must be a non negative integer */
static int	is_num(char *str)
{
	int	i;

	i = 0;
	if (!str[0])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	parse_args(t_sim *s, char **av)
{
	int	i;

	i = 1;
	while (i <= 7)
	{
		if (!is_num(av[i]) || strlen(av[i]) > 9)
			return (1);
		i++;
	}
	s->nb_coders = atoi(av[1]);
	s->t_burnout = atoi(av[2]);
	s->t_compile = atoi(av[3]);
	s->t_debug = atoi(av[4]);
	s->t_refactor = atoi(av[5]);
	s->min_compiles = atoi(av[6]);
	s->t_cooldown = atoi(av[7]);
	if (s->nb_coders < 1)
		return (1);
	if (strcmp(av[8], "fifo") == 0)
		s->use_edf = 0;
	else if (strcmp(av[8], "edf") == 0)
		s->use_edf = 1;
	else
		return (1);
	return (0);
}

static int	setup(t_sim *s)
{
	int	i;

	s->sim_start = now_ms();
	if (alloc_sim_arrays(s))
		return (1);
	pthread_mutex_init(&s->data_lock, NULL);
	pthread_mutex_init(&s->print_lock, NULL);
	i = 0;
	while (i < s->nb_coders)
	{
		init_coder(s, i);
		if (init_dongle(s, i))
			return (1);
		i++;
	}
	if (s->min_compiles == 0)
		s->all_done = 1;
	return (0);
}

static void	run(t_sim *s)
{
	pthread_t	monitor;
	int			has_monitor;
	int			started;

	has_monitor = 0;
	started = 0;
	while (started < s->nb_coders)
	{
		if (pthread_create(&s->c[started].tid, NULL, coder_life,
				&s->c[started]))
		{
			set_stop(s);
			break ;
		}
		started++;
	}
	has_monitor = pthread_create(&monitor, NULL, watch, s);
	if (has_monitor)
		set_stop(s);
	while (started > 0)
		pthread_join(s->c[--started].tid, NULL);
	if (has_monitor)
		pthread_join(monitor, NULL);
}

int	main(int ac, char **av)
{
	t_sim	s;

	memset(&s, 0, sizeof(s));
	if (ac != 9 || parse_args(&s, av))
		return (arg_error());
	if (setup(&s))
		return (1);
	run(&s);
	cleanup(&s);
	return (0);
}
