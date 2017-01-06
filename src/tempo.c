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



/**********************************/
/** Entries and tools for timers **/
/**********************************/

struct timer_entry *entries;
static struct timer_entry *zeroEntries;
static struct timeval zeroTimeval;

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

void removeFirstEntry(){
    if (entries->next != NULL)
        entries = entries->next;
    else if(TVEQ(entries->tv, zeroTimeval))
        entries = zeroEntries;
}

static void alrmHandler(int signum){
    struct itimerval timer;
    struct timeval now;
    struct timeval then;
    struct timer_entry *current = entries;

    // Getting actual time
    gettimeofday(&now, NULL);

    // Loop on first elements
    while(TVEQ(current->tv, current->next->tv)){
        // Pushing event
        sdl_push_event(current->data);
        // Going to next element
        current = current->next;
        // Removing first entry, used
        removeFirstEntry();
    }

    // New alarm
    then.tv_sec = current->tv.tv_sec - now.tv_sec;
    then.tv_usec = current->tv.tv_usec - now.tv_usec;
    // If subtraction has made the
    if(then.tv_usec < 0)
    {
        then.tv_sec -= 1;
        then.tv_usec += 1000000;
    }
    // If then is negative
    if((then.tv_sec <= 0) && (then.tv_usec <= 0))
    {
        fprintf(stderr, "ERROR : A TIMER SCHEDULE IS <=0 !");
        fflush(stderr);
    }
    else {
        timer.it_interval = zeroTimeval;
        timer.it_value = then;
        setitimer(ITIMER_REAL, &timer, NULL);
    }

    // OLD
    pthread_t my_id;
    my_id = pthread_self();
    printf("\nTHREAD [%u] : Handling alarm\n", (unsigned int)my_id);

}

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
    printf("\n** i ** PID : %d ** i **\n", getpid());
    fflush(stdout);
    // Deamon
    while(1){
        sigsuspend(&new_set);
    }
}

bool isEmptyTimeval(struct timeval* tv){
    struct timeval emptyTV;
    emptyTV.tv_sec = 0;
    emptyTV.tv_usec = 0;
    double emptyTimeval = emptyTV.tv_sec + ( emptyTV.tv_usec / 1000000 );
    double tvDouble = tv->tv_sec + ( tv->tv_usec / 1000000 );

    if(tvDouble - emptyTimeval == emptyTimeval)
        return true;
    else
        return false;
}

// timer_init returns 1 if timers are fully implemented, 0 otherwise
int timer_init (void)
{
    // Init some variables for timers
    zeroTimeval.tv_sec = 0;
    zeroTimeval.tv_usec = 0;
    zeroEntries->tv = zeroTimeval;
    zeroEntries->data = NULL;
    zeroEntries->next = NULL;

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

  return 0; // Implementation not ready
}

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

void swapEntries(struct timer_entry *a, struct timer_entry *b){
    struct timer_entry *tmp = a;
    a->tv = b->tv;
    a->data = b->data;
    a->next = b->next;
    b->tv = tmp->tv;
    b->data = tmp->data;
    b->next = tmp->next;
}

void timer_set (Uint32 delay, void *param)
{
    // Vars
    bool ok = false;
    bool first = true;
    struct timeval newTimer = computeTimevalTimer(delay);
    struct timer_entry *currentEntry = entries;
    struct timer_entry *newEntry;
    newEntry->tv = newTimer;
    newEntry->data = param;
    newEntry->next = NULL;
    // Finding the place to insert
    while(!ok){
        // INSERTION : Inférieur à la case, effectuer un échange
        if(TVLESS(newTimer, currentEntry->tv)){
            newEntry->next = currentEntry;
            swapEntries(currentEntry, newEntry);
            ok = true;
        }
        // INSERTION : Dernier élément de chaine atteint
        else if(currentEntry->next == NULL){
            currentEntry->next = newEntry;
            ok = true;
        }
        // Next element
        else {
            first = false;
            currentEntry = currentEntry->next;
        }
    }
    if(first){
        struct itimerval timer;
        setitimer(ITIMER_REAL, &timer, NULL);
    }
}


//#endif
