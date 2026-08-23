#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/time.h>

# define MSG_TAKEN     "has taken a dongle"
# define MSG_COMPILE   "is compiling"
# define MSG_DEBUG     "is debugging"
# define MSG_REFACTOR  "is refactoring"
# define MSG_BURN      "burned out"

typedef struct s_sim    t_sim;

/* one pending request for a dongle */
typedef struct s_req
{
    long long   key;
    long long   arrival;
    int         coder;
}   t_req;

/* array based min-heap used as the dongle waiting queue */
typedef struct s_heap
{
    t_req      *tab;
    int         size;
}   t_heap;

/* one USB dongle shared by two neighbour coders */
typedef struct s_dongle
{
    int             busy;
    int             owner;
    long long       avail_at;   /* when its cooldown ends */
    t_heap          q;
    pthread_mutex_t lock;
    pthread_cond_t  cond;
}   t_dongle;

/* one coder thread */
typedef struct s_coder
{
    pthread_t       tid;
    int             id;
    int             first;
    int             second;
    long long       deadline;   /* last compile start + time_to_burnout */
    int             compiles;   /* how many times he compiled so far */
    t_sim          *sim;
}   t_coder;

struct s_sim
{
    int             nb_coders;
    long long       t_burnout;
    long long       t_compile;
    long long       t_debug;
    long long       t_refactor;
    int             min_compiles;
    long long       t_cooldown;
    int             use_edf;    /* 0 = fifo, 1 = earliest deadline first */
    long long       sim_start;
    int             burnout;    /* someone burned out */
    int             all_done;   /* everyone compiled enough */
    pthread_mutex_t data_lock;
    pthread_mutex_t print_lock;
    t_dongle       *d;
    t_coder        *c;
};

/* utils.c */
long long   now_ms(void);
void        msleep(long long ms);
void        log_state(t_sim *s, int id, const char *msg);
int         arg_error(void);
int         sim_over(t_sim *s);
void        set_stop(t_sim *s);

/* heap.c */
int         heap_init(t_heap *h, int cap);
void        heap_free(t_heap *h);
void        heap_push(t_heap *h, t_req r);
t_req       heap_pop(t_heap *h);
void        heap_remove(t_heap *h, int coder);

/* dongle.c */
int         take_dongle(t_sim *s, t_coder *me, int idx);
void        drop_dongle(t_sim *s, int idx);
void        wake_all(t_sim *s);

/* coder.c */
void       *coder_life(void *arg);

/* monitor.c */
void       *watch(void *arg);

#endif
