/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongles_coders.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdait-s <abdait-s@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:07:31 by abdait-s          #+#    #+#             */
/*   Updated: 2026/08/26 11:08:12 by abdait-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	init_coder(t_sim *s, int i)
{
	s->c[i].id = i + 1;
	s->c[i].first = i;
	s->c[i].second = (i + 1) % s->nb_coders;
	if (s->c[i].second < s->c[i].first)
	{
		s->c[i].first = s->c[i].second;
		s->c[i].second = i;
	}
	s->c[i].deadline = s->sim_start + s->t_burnout;
	s->c[i].sim = s;
}

int	alloc_sim_arrays(t_sim *s)
{
	s->d = malloc(sizeof(t_dongle) * s->nb_coders);
	s->c = malloc(sizeof(t_coder) * s->nb_coders);
	if (!s->d || !s->c)
		return (1);
	memset(s->d, 0, sizeof(t_dongle) * s->nb_coders);
	memset(s->c, 0, sizeof(t_coder) * s->nb_coders);
	return (0);
}

int	init_dongle(t_sim *s, int i)
{
	pthread_mutex_init(&s->d[i].lock, NULL);
	pthread_cond_init(&s->d[i].cond, NULL);
	if (!heap_init(&s->d[i].q, s->nb_coders))
	{
		heap_free(&s->d[i].q);
		return (1);
	}
	return (0);
}
