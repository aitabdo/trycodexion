#include "codexion.h"

/*
** the monitor polls the coders every millisecond. when one of them passes
** its burnout deadline it logs it right away and stops everything.
** polling at 1ms keeps us way inside the 10ms tolerance.
*/

void    *watch(void *arg)
{
    t_sim       *s;
    long long   now;
    int         victim;
    int         burned;
    int         i;

    s = (t_sim *)arg;
    while (!sim_over(s))
    {
        usleep(1000);
        burned = 0;
        victim = 0;
        pthread_mutex_lock(&s->data_lock);
        if (!s->burnout && !s->all_done)
        {
            now = now_ms();
            i = 0;
            while (i < s->nb_coders)
            {
                if (now > s->c[i].deadline)
                {
                    s->burnout = 1;
                    burned = 1;
                    victim = s->c[i].id;
                    break ;
                }
                i++;
            }
        }
        pthread_mutex_unlock(&s->data_lock);
        if (burned)
        {
            log_state(s, victim, MSG_BURN);
            wake_all(s);
        }
    }
    return (NULL);
}
