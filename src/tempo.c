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

struct timeval TVS[100] = { 0 };
int lastTimerIndex = -1;

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

static void alrmHandler(int signum){
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

/********************************************************/
/*** Function returning the double value of a timeval ***/
/********************************************************/
double timevalToDouble(struct timeval tv){
    return tv.tv_sec + ( tv.tv_usec / 1000000 );
}

/*****************************************/
/*** Function substracting two timeval ***/
/*****************************************/
int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y) {
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

bool isEmptyTimeval(struct timeval tv){
    struct timeval emptyTV;
    emptyTV.tv_sec = 0;
    emptyTV.tv_usec = 0;
    double emptyTimeval = emptyTV.tv_sec + ( emptyTV.tv_usec / 1000000 );
    double tvDouble = tv.tv_sec + ( tv.tv_usec / 1000000 );

    if(tvDouble - emptyTimeval == emptyTimeval)
        return true;
    else
        return false;
}

// timer_init returns 1 if timers are fully implemented, 0 otherwise
int timer_init (void)
{
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
//        return 1;
    }

  return 0; // Implementation not ready
}

struct timeval computeTimeval(Uint32 delay){
    // TODO: compute timeval from delay (gettime + delay converti
}

void timer_set (Uint32 delay, void *param)
{
    int i = 0;
    bool ok = true;
    struct timeval newTimer = computeTimeval(delay);
    // Finding the place to insert
    while(ok){
        // Si la case est libre ou si on est plus petit que le timer courant
        if(isEmptyTimeval(TVS[i]) || timeval_subtract(*(struct timeval){0}, )){

        }

    }
    setitimer(ITIMER_VIRTUAL);
  // TODO
}


//#endif
