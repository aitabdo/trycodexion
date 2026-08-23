#include "codexion.h"

/*
** life of a coder: take two dongles, compile, put them back,
** debug, refactor, then try to compile again.
*/

static void start_compile(t_coder *me)
{
    t_sim   *s;
    int     i;

    s = me->sim;
    pthread_mutex_lock(&s->data_lock);
    me->deadline = now_ms() + s->t_burnout;
    me->compiles++;
    if (!s->all_done && me->compiles >= s->min_compiles && s->min_compiles > 0)
    {
        i = 0;
        while (i < s->nb_coders && s->c[i].compiles >= s->min_compiles)
            i++;
        if (i == s->nb_coders)
            s->all_done = 1;
    }
    pthread_mutex_unlock(&s->data_lock);
}

static void retire(t_coder *me)
{
    pthread_mutex_lock(&me->sim->data_lock);
    me->deadline = 0x7FFFFFFFFFFFFFFFLL;    /* no deadline anymore */
    pthread_mutex_unlock(&me->sim->data_lock);
}

void    *coder_life(void *arg)
{
    t_coder *me;
    t_sim   *s;

    me = (t_coder *)arg;
    s = me->sim;
    while (!sim_over(s))
    {
        if (take_dongle(s, me, me->first))
            break ;
        if (take_dongle(s, me, me->second))
        {
            drop_dongle(s, me->first);
            break ;
        }
        start_compile(me);
        log_state(s, me->id, MSG_COMPILE);
        msleep(s->t_compile);
        drop_dongle(s, me->second);
        drop_dongle(s, me->first);
        if (s->min_compiles > 0 && me->compiles >= s->min_compiles)
        {
            retire(me);
            break ;
        }
        if (sim_over(s))
            break ;
        log_state(s, me->id, MSG_DEBUG);
        msleep(s->t_debug);
        if (sim_over(s))
            break ;
        log_state(s, me->id, MSG_REFACTOR);
        msleep(s->t_refactor);
    }
    return (NULL);
}
