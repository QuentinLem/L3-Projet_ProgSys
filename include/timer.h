
#ifndef TIMER_IS_DEF
#define TIMER_IS_DEF

struct timer_entry {
    struct timeval tv;
    void *data;
    struct timer_entry *next;
};

int timer_init (void);
void timer_set (Uint32 delay, void *param);

void sdl_push_event (void *param);

// Compare two timeval
#define TVLESS(a,b) ((a).tv_sec == (b).tv_sec ? ((a).tv_usec < (b).tv_usec) : ((a).tv_sec < (b).tv_sec))

#define TVEQ(a,b) ((a).tv_sec == (b).tv_sec ? ((a).tv_usec == (b).tv_usec) : (0))

#endif
