#include "codexion.h"

/*
** min-heap of requests. order: key (arrival for fifo, deadline for edf),
** then arrival, then coder number so ties are always broken the same way.
*/

static int before(t_req a, t_req b)
{
    if (a.key != b.key)
        return (a.key < b.key);
    if (a.arrival != b.arrival)
        return (a.arrival < b.arrival);
    return (a.coder < b.coder);
}

static void swap(t_req *a, t_req *b)
{
    t_req   tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

int heap_init(t_heap *h, int cap)
{
    h->size = 0;
    h->tab = malloc(sizeof(t_req) * cap);
    return (h->tab != NULL);
}

void    heap_free(t_heap *h)
{
    free(h->tab);
    h->tab = NULL;
}

void    heap_push(t_heap *h, t_req r)
{
    int i;

    i = h->size;
    h->tab[i] = r;
    h->size++;
    while (i > 0 && before(h->tab[i], h->tab[(i - 1) / 2]))
    {
        swap(&h->tab[i], &h->tab[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

static void sift_down(t_heap *h, int i)
{
    int l;
    int r;
    int m;

    while (1)
    {
        l = 2 * i + 1;
        r = 2 * i + 2;
        m = i;
        if (l < h->size && before(h->tab[l], h->tab[m]))
            m = l;
        if (r < h->size && before(h->tab[r], h->tab[m]))
            m = r;
        if (m == i)
            break ;
        swap(&h->tab[i], &h->tab[m]);
        i = m;
    }
}

t_req   heap_pop(t_heap *h)
{
    t_req   top;

    top = h->tab[0];
    h->size--;
    h->tab[0] = h->tab[h->size];
    sift_down(h, 0);
    return (top);
}

/* remove my own request from the queue (used when giving up) */
void    heap_remove(t_heap *h, int coder)
{
    int i;

    i = 0;
    while (i < h->size && h->tab[i].coder != coder)
        i++;
    if (i == h->size)
        return ;
    h->size--;
    h->tab[i] = h->tab[h->size];
    while (i > 0 && before(h->tab[i], h->tab[(i - 1) / 2]))
    {
        swap(&h->tab[i], &h->tab[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
    sift_down(h, i);
}
