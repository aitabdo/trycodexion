/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdait-s <abdait-s@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:07:07 by abdait-s          #+#    #+#             */
/*   Updated: 2026/08/26 11:07:48 by abdait-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
