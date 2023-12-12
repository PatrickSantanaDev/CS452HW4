#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <X11/Xlib.h>

#include "lawn.h"
#define LAWNIMP
#include "lawnimp.h"
#undef LAWNIMP
#include "error.h"

/**
 * Threaded entry point that manages execution of the graphics representation for the lawn. 
 *
 * @param l void pointer representing LawnRep and state
 * 
 * @return pointer to the lawnimp_run return, sucessful return 0 terminates thread. 
 */
static void *run(void *l)
{
  LawnRep r = (LawnRep)l;
  return lawnimp_run(r);
}

/**
 * Configures the signal mask to block all signals temporarily
 *
 * @param new_signal_mask A set where new signals will be stored.
 * @param prev_signal_mask A set where previous signals will be saved.
 *
 */
static void configure_signal_mask(sigset_t *new_signal_mask, sigset_t *prev_signal_mask)
{
  sigfillset(new_signal_mask);
  pthread_sigmask(SIG_BLOCK, new_signal_mask, prev_signal_mask);
}

/**
 * Creates and initializes a new Lawn object and sets up associated thread
 *
 * @param lawnsize size of the lawn.
 * @param molesize size of the moles.
 *
 * @return Returns a newly created Lawn object.
 */
extern Lawn lawn_new(int lawnsize, int molesize)
{
  // assign default values for lawnsize and molesize
  if (!lawnsize)
    lawnsize = 40;
  if (!molesize)
    molesize = 15;

  // initialize Xlib library for threaded use, multiple threads will access Xlib.
  XInitThreads();

  // allocate memory for new LawnRep and ensure success
  LawnRep lawn = (LawnRep)malloc(sizeof(*lawn));
  if (!lawn)
    ERROR("malloc() failed");

  // initialize new LawnRep with vals
  lawn->lawnsize = lawnsize;
  lawn->molesize = molesize;

  // create new window of calculated sizes for lawn
  lawn->window = lawnimp_new(lawnsize, molesize);

  // declare and initialize thread attributes
  pthread_attr_t tattr;
  pthread_attr_init(&tattr);

  // configure signal masks to block all signals temporarily
  sigset_t new_signal_mask, prev_signal_mask;
  configure_signal_mask(&new_signal_mask, &prev_signal_mask);

  // create new thread executing run and check for success
  if (pthread_create(&lawn->thread, &tattr, run, lawn))
    ERROR("pthread_create() failed: %s", strerror(errno));

  // reset signal mask to previous state, unblocking temporarily blocked signals
  pthread_sigmask(SIG_SETMASK, &prev_signal_mask, 0);

  // clean up thread attributes object
  pthread_attr_destroy(&tattr);

  // return newly created and initialized LawnRep structure.
  return lawn;
}

/**
 * Frees up the resources held by a Lawn object and cancels its associated thread.
 *
 * @param l A Lawn object to be freed.
 *
 */
extern void lawn_free(Lawn l)
{
  LawnRep r = (LawnRep)l;
  lawnimp_free(r->window);
  pthread_cancel(r->thread);
  if (pthread_join(r->thread, 0))
    ERROR("pthread_join() failed: %s", strerror(errno));
  free(r);
}
