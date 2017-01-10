#define _XOPEN_SOURCE 600

#include <SDL.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

#include "timer.h"
#include "../include/timer.h"

// Timer_entry struct
struct timer_entry {
    struct timeval tv;
    void *data;
    struct timer_entry *next;
};

// Compare two timeval
// Less operator
#define TVLESS(a,b) ((a).tv_sec == (b).tv_sec ? ((a).tv_usec < (b).tv_usec) : ((a).tv_sec < (b).tv_sec))
// Less or equal operator
#define TVLESSEQ(a,b) ((a).tv_sec == (b).tv_sec ? ((a).tv_usec <= (b).tv_usec) : ((a).tv_sec <= (b).tv_sec))


/**********************************/
/** Entries and tools for timers **/
/**********************************/
struct timer_entry *entries;
static struct timer_entry *zeroEntry;
static struct timeval zeroTimeval;

// Log for alrm handle
bool logCreated = false;
// Mutex for entries modification
pthread_mutex_t entriesLock;


// Return number of elapsed µsec since..    . a long time ago
static unsigned long get_time (void)
{
  struct timeval tv;

  gettimeofday (&tv ,NULL);

  // Only count seconds since beginning of 2016 (not jan 1st, 1970)
  tv.tv_sec -= 3600UL * 24 * 365 * 46;

  return tv.tv_sec * 1000000UL + tv.tv_usec;
}

//#ifdef PADAWAN

// Function used to substract timevals into result arg and returning if res is negative or not
int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}


// Sigalrm handler
static void alrmHandler(int signum){
    FILE *file;
    if(!logCreated){
        file = fopen("log.txt", "w");
        logCreated = true;
    }
    else
        file = fopen("log.txt", "a");

    struct itimerval timer;
    struct timeval now;
    struct timeval then;
    struct timer_entry *current = entries;
    fputs("### ALRM : ENTERING HANDLER", file);
    // Getting actual time
    gettimeofday(&now, NULL);

    // Trigger mutex on entries for this work
    pthread_mutex_lock(&entriesLock);
    while (entries)
    {
        gettimeofday (&now, NULL);
        current = entries;
        if (TVLESSEQ (current->tv, now))
        {
            entries = entries->next;
            sdl_push_event(current->data);
            free (current);
        }
        else
            break;
    }
    if (entries)
    {
        // Negative next timer
        if(timeval_subtract(&then, &entries->tv, &now)){
            fputs ("Scheduling for <=0 time?!\n", file);
        }
        // non negative
        else {
            timer.it_interval = zeroTimeval;
            timer.it_value = then;
            setitimer (ITIMER_REAL, &timer, NULL);
        }
    }
    // Release mutex
    pthread_mutex_unlock(&entriesLock);

    // OLD
    pthread_t my_id;
    my_id = pthread_self();
    printf("\nTHREAD [%u] : Handling alarm\n", (unsigned int)my_id);

    // CLOSING LOG
    fputs("### ALRM : EVENT HAS BEEN HANDLED", file);
    fclose(file);
}

// Deamon used to manage signals
void * alrmDeamon(void *param){
    sigset_t new_set;
    // Blocking all
    sigfillset(&new_set);
    // Except SIGALRM
    sigdelset(&new_set, SIGALRM);
    // Applying mask
    sigprocmask(SIG_SETMASK,&new_set,NULL);

    // Handler
    struct sigaction alrmAction;
    alrmAction.sa_handler = alrmHandler;
    sigemptyset(&alrmAction.sa_mask);
    alrmAction.sa_flags = SA_RESTART;

    if (sigaction(SIGALRM, &alrmAction, NULL) == -1){
        printf("\n** ERR ** Error linking sigaction (SIGALRM) ** ERR **\n");
    }

    // Deamon
    while(1){
        sigsuspend(&new_set);
    }
}

// timer_init returns 1 if timers are fully implemented, 0 otherwise
int timer_init (void)
{
    // Init some variables for timers
    zeroTimeval.tv_sec = 0;
    zeroTimeval.tv_usec = 0;
    zeroEntry = (struct timer_entry*) malloc (sizeof (struct timer_entry));
    zeroEntry->tv = zeroTimeval;
    zeroEntry->data = NULL;
    zeroEntry->next = NULL;
    entries = (struct timer_entry*) malloc (sizeof (struct timer_entry));
    // Mutex
    pthread_mutex_init(&entriesLock, NULL);

    // Block SIGALRM reception
    sigset_t new_set;
    sigemptyset(&new_set);
    if(sigaddset(&new_set,SIGALRM)==0)
    {
        printf("Sigaddset has worked for Sigalrm\n");
    }
    // Using new mask
    sigprocmask(SIG_SETMASK,&new_set,NULL);

    // Create thread
    pthread_t alrmThread;
    if(pthread_create(&alrmThread, NULL, &alrmDeamon, NULL)){
        printf("\nError creating thread\n");
    }

  return 1; // Implementation not ready
}

// Function used to compute the timeval value of a timer from the delay in ms
struct timeval computeTimevalTimer(Uint32 delay){
    // Getting current timeval
    struct timeval tv;
    gettimeofday(&tv, NULL);
    // Add vars
    int addSec, addUSec;
    // Computing
    addSec = (delay - delay % 1000) / 1000 ;
    addUSec = (delay % 1000) * 1000;
    tv.tv_sec += addSec;
    tv.tv_usec += addUSec;
    return tv;
}

// Function used to set timer
void timer_set (Uint32 delay, void *param)
{
    // Vars
    bool first = false;
    struct timeval newTimer = computeTimevalTimer(delay);
    struct timer_entry *currentEntry = entries;
    struct timer_entry *newEntry = NULL;

    struct timeval diff;
    struct timeval now;
    gettimeofday(&now, NULL);
    timeval_subtract(&diff, &newTimer, &now);
    pthread_mutex_lock(&entriesLock);

    // Finding place to insert
    while (currentEntry)
    {
        if (TVLESS (newTimer, currentEntry->tv))
            break;
        newEntry = currentEntry;
        currentEntry = currentEntry->next;
    };

    // Place if not first
    if (newEntry)
    {
        newEntry->next =
                (struct timer_entry *) malloc (sizeof (struct timer_entry));
        newEntry = newEntry->next;
    }
    // If first element
    else
    {
        newEntry = (struct timer_entry *) malloc (sizeof (struct timer_entry));
        entries = newEntry;
        first = true;
    }
    // Affecting vars
    newEntry->tv = newTimer;
    newEntry->data = param;
    newEntry->next = currentEntry;
    if (first)
    {
        struct itimerval timer;
        timer.it_value = diff;
        // Interval set to zero to stop timer when triggered
        timer.it_interval = zeroTimeval;
        setitimer(ITIMER_REAL, &timer, NULL);
    }
    pthread_mutex_unlock(&entriesLock);

}


//#endif
