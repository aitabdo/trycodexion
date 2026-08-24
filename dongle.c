/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdait-s <abdait-s@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 11:49:25 by abdait-s          #+#    #+#             */
/*   Updated: 2026/08/24 12:00:52 by abdait-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** A dongle is granted only to the request at the top of its queue
** (fifo or edf depending on the scheduler), and only once it is free
** and its cooldown has expired. Waiting is done on a condition variable
** protected by the dongle mutex.
*/

static int	my_turn(t_dongle *d, int id)
{
	return (d->q.size > 0 && d->q.tab[0].coder == id);
}

int	take_dongle(t_sim *s, t_coder *me, int idx)
{
	t_dongle		*d;
	struct timespec	ts;
	t_req			req;

	d = &s->d[idx];
	pthread_mutex_lock(&d->lock);
	req.arrival = now_ms();
	req.key = me->deadline;
	if (!s->use_edf)
		req.key = req.arrival;
	req.coder = me->id;
	heap_push(&d->q, req);
	while (!my_turn(d, me->id) || d->busy || now_ms() < d->avail_at)
	{
		if (sim_over(s))
		{
			heap_remove(&d->q, me->id);
			pthread_mutex_unlock(&d->lock);
			return (1);
		}
		if (now_ms() < d->avail_at)
		{
			ts.tv_sec = (time_t)(d->avail_at / 1000);
			ts.tv_nsec = (long)(d->avail_at % 1000) * 1000000L;
			pthread_cond_timedwait(&d->cond, &d->lock, &ts);
		}
		else
			pthread_cond_wait(&d->cond, &d->lock);
	}
	heap_pop(&d->q);
	d->busy = 1;
	d->owner = me->id;
	pthread_mutex_unlock(&d->lock);
	log_state(s, me->id, MSG_TAKEN);
	return (0);
}

void	drop_dongle(t_sim *s, int idx)
{
	t_dongle	*d;

	d = &s->d[idx];
	pthread_mutex_lock(&d->lock);
	d->busy = 0;
	d->owner = 0;
	d->avail_at = now_ms() + s->t_cooldown;
	pthread_cond_broadcast(&d->cond);
	pthread_mutex_unlock(&d->lock);
}

/* wake up every waiter so they can notice the simulation is over */
void	wake_all(t_sim *s)
{
	int	i;

	i = 0;
	while (i < s->nb_coders)
	{
		pthread_mutex_lock(&s->d[i].lock);
		pthread_cond_broadcast(&s->d[i].cond);
		pthread_mutex_unlock(&s->d[i].lock);
		i++;
	}
}
