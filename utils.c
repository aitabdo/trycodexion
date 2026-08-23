#include "codexion.h"


long long   now_ms(void)
{
    struct timeval  tv;

    gettimeofday(&tv, NULL);
    return ((long long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void    msleep(long long ms)
{
    usleep(ms * 1000);
}

/* all logs go through here so lines can never mix together */
void    log_state(t_sim *s, int id, const char *msg)
{
    pthread_mutex_lock(&s->print_lock);
    if (!s->burnout || strcmp(msg, MSG_BURN) == 0)
        printf("%lld %d %s\n", now_ms() - s->sim_start, id, msg);
    pthread_mutex_unlock(&s->print_lock);
}

int arg_error(void)
{
    fprintf(stderr, "parsing error ");
    return (1);
}

int sim_over(t_sim *s)
{
    int over;

    pthread_mutex_lock(&s->data_lock);
    over = s->burnout || s->all_done;
    pthread_mutex_unlock(&s->data_lock);
    return (over);
}

/* flag the end of the simulation and wake everyone stuck on a dongle */
void    set_stop(t_sim *s)
{
    pthread_mutex_lock(&s->data_lock);
    s->burnout = 1;
    pthread_mutex_unlock(&s->data_lock);
    wake_all(s);
}
