#if defined(LIBEV_EMBED)
#include "ev.c"
#else /* !LIBEV_EMBED */
#include "ev.h"

#ifndef _WIN32
#include <signal.h>
#endif /* !_WIN32 */

#endif /* LIBEV_EMBED */

#ifndef _WIN32

static struct sigaction libev_sigchld;
/*
 * Track the state of whether we have installed
 * the libev sigchld handler specifically.
 * If it's non-zero, libev_sigchld will be valid and set to the action
 * that libev needs to do.
 * If it's 1, we need to install libev_sigchld to make libev
 * child handlers work (on request).
 */
static int sigchld_state = 0;

static struct ev_loop* gevent_ev_default_loop(unsigned int flags)
{
    struct ev_loop* result;
    struct sigaction tmp;

    if (sigchld_state)
        return ev_default_loop(flags);

    //  Request the old SIGCHLD handler
    sigaction(SIGCHLD, NULL, &tmp);
    // Get the loop, which will install a SIGCHLD handler
    result = ev_default_loop(flags);
    // XXX what if SIGCHLD received there?
    // Now restore the previous SIGCHLD handler
    sigaction(SIGCHLD, &tmp, &libev_sigchld);
    sigchld_state = 1;
    return result;
}


static void gevent_install_sigchld_handler(void) {
    if (sigchld_state == 1) {
        sigaction(SIGCHLD, &libev_sigchld, NULL);
        sigchld_state = 2;
    }
}

static void gevent_reset_sigchld_handler(void) {
   // We could have any state at this point, depending on
   // whether the default loop has been used. If it has,
   // then always be in state 1 ("need to install)
   if (sigchld_state) {
       sigchld_state = 1;
   }
}

#else /* !_WIN32 */

#define gevent_ev_default_loop ev_default_loop
static void gevent_install_sigchld_handler(void) { }
static void gevent_reset_sigchld_handler(void) { }

// Fake child functions that we can link to.
static void ev_child_start(struct ev_loop* loop, ev_child* w) {};
static void ev_child_stop(struct ev_loop* loop, ev_child* w) {};

#endif /* _WIN32 */
