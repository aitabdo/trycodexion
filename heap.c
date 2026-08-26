/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdait-s <abdait-s@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 11:49:28 by abdait-s          #+#    #+#             */
/*   Updated: 2026/08/26 10:35:52 by abdait-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Two-slot min-heap: a dongle is shared by only two adjacent coders.
** Order: key, then arrival, then coder number for deterministic ties.
*/

static int	before(t_req a, t_req b)
{
	if (a.key != b.key)
		return (a.key < b.key);
	if (a.arrival != b.arrival)
		return (a.arrival < b.arrival);
	return (a.coder < b.coder);
}

static void	swap(t_req *a, t_req *b)
{
	t_req	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

int	heap_init(t_heap *h, int cap)
{
	h->size = 0;
	if (cap > 2)
		cap = 2;
	h->tab = malloc(sizeof(t_req) * cap);
	return (h->tab != NULL);
}

void	heap_push(t_heap *h, t_req r)
{
	h->tab[h->size] = r;
	h->size++;
	if (h->size == 2 && before(h->tab[1], h->tab[0]))
		swap(&h->tab[0], &h->tab[1]);
}

t_req	heap_pop(t_heap *h)
{
	t_req	top;

	top = h->tab[0];
	h->size--;
	if (h->size == 1)
		h->tab[0] = h->tab[1];
	return (top);
}
