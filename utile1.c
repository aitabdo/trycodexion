/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utile1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdait-s <abdait-s@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 12:06:48 by abdait-s          #+#    #+#             */
/*   Updated: 2026/08/24 12:09:37 by abdait-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	arg_error(void)
{
	fprintf(stderr, "parsing error ");
	return (1);
}

void	cleanup(t_sim *s)
{
	int	i;

	i = 0;
	while (i < s->nb_coders)
	{
		heap_free(&s->d[i].q);
		pthread_cond_destroy(&s->d[i].cond);
		pthread_mutex_destroy(&s->d[i].lock);
		i++;
	}
	pthread_mutex_destroy(&s->data_lock);
	pthread_mutex_destroy(&s->print_lock);
	free(s->d);
	free(s->c);
}
