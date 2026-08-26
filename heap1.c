/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap1.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdait-s <abdait-s@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 10:36:16 by abdait-s          #+#    #+#             */
/*   Updated: 2026/08/26 10:36:16 by abdait-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	heap_free(t_heap *h)
{
	free(h->tab);
	h->tab = NULL;
}

/* remove my own request from the queue (used when giving up) */
void	heap_remove(t_heap *h, int coder)
{
	int	i;

	i = 0;
	while (i < h->size && h->tab[i].coder != coder)
		i++;
	if (i == h->size)
		return ;
	h->size--;
	if (i == 0 && h->size == 1)
		h->tab[0] = h->tab[1];
}
