/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdait-s <abdait-s@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 11:49:33 by abdait-s          #+#    #+#             */
/*   Updated: 2026/08/26 11:05:34 by abdait-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** the monitor polls the coders every millisecond. when one of them passes
** its burnout deadline it logs it right away and stops everything.
** polling at 1ms keeps us way inside the 10ms tolerance.
*/
static int	find_burned_coder(t_sim *s)
{
	long long	now;
	int			i;

	now = now_ms();
	i = 0;
	while (i < s->nb_coders)
	{
		if (now > s->c[i].deadline)
		{
			s->burnout = 1;
			return (s->c[i].id);
		}
		i++;
	}
	return (0);
}

void	*watch(void *arg)
{
	t_sim	*s;
	int		victim;

	s = (t_sim *)arg;
	while (!sim_over(s))
	{
		usleep(1000);
		victim = 0;
		pthread_mutex_lock(&s->data_lock);
		if (!s->burnout && !s->all_done)
			victim = find_burned_coder(s);
		pthread_mutex_unlock(&s->data_lock);
		if (victim)
		{
			log_state(s, victim, MSG_BURN);
			wake_all(s);
		}
	}
	return (NULL);
}
