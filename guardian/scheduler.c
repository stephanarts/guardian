/*-
 * Copyright (c) 2012 Stephan Arts. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY [LICENSOR] "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE,DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#include <pthread.h>

#include <semaphore.h>

#include <time.h>

#include <libguardian/libguardian.h>

/** Define 5 Second interval */
#define INTERVAL 2
static int n_sources = 0;
static GuardianSource **sources = NULL;

/** 
 * Requests from sockets are handled in threads,
 * because 'queries' can take an arbitrary time
 * to return.
 */
#define         THREADPOOL_SIZE 10
static int      thread_count = 0;
pthread_mutex_t thread_count_mutex;
sem_t           max_threads_sem;

pthread_mutex_t thread_busy_mutex;

pthread_cond_t  t_max_threads_cv;

static void *
_guardian_scheduler_thread_run (void *__arg);

static void *
guardian_scheduler_concept (void *__arg);

void
guardian_scheduler_main ( void )
{
    struct timespec s_t;
    struct timespec e_t;
    struct timespec sl_t;
    int i;
    pthread_t thread;

    pthread_mutex_init (
            &thread_count_mutex,
            NULL );

    sem_init (
            &max_threads_sem,
            0,
            0 );

    pthread_cond_init (
            &t_max_threads_cv,
            NULL );

    pthread_mutex_init (
            &thread_busy_mutex,
            NULL );

    /** Spawn a new worker thread */
    pthread_create (
            &thread,
            NULL,
            guardian_scheduler_concept,
            NULL);

    while (1)
    {
        clock_gettime (CLOCK_REALTIME, &s_t); 

        for (i = 0; i < n_sources; ++i)
        {
            guardian_source_update (sources[i]);
        }

        clock_gettime (CLOCK_REALTIME, &e_t); 

        sl_t.tv_sec = INTERVAL - (e_t.tv_sec - s_t.tv_sec);
        sl_t.tv_nsec = 0;

        if (clock_nanosleep (
                CLOCK_REALTIME,
                0,
                &sl_t,
                NULL))
        {
            return;
        }
    }
}

/**
 * guardian_scheduler_add_source:
 * @source: Source object
 *
 * Adds a source to be polled at regular intervals.
 */
void
guardian_scheduler_add_source ( GuardianSource *source)
{
    GuardianSource **_sources;
    int i;

    if (sources == NULL)
    {
        sources = malloc (sizeof(GuardianSource *) * 2);
        sources[0] = source;
        sources[1] = NULL;
        n_sources = 1;
        return;
    }

    _sources = malloc (sizeof(GuardianSource *) * n_sources + 2);


    for(i = 0; i < n_sources; ++i)
    {
        _sources[i] = sources[i];
    }
    _sources[i] = source;

    free (sources);
    sources = _sources;

    n_sources++;
}

static void *
guardian_scheduler_concept (void *__arg)
{
    pthread_t thread;

    while (1)
    {
        printf(",[%d]\n", thread_count);

        pthread_mutex_lock (&thread_count_mutex);

        /** Check the number of running threads */
        if (thread_count < THREADPOOL_SIZE )
        {
            /** Increase the running thread-count */
            thread_count++;

            /** Spawn a new worker thread */
            pthread_create (
                    &thread,
                    NULL,
                    _guardian_scheduler_thread_run,
                    NULL);
            /** Unlock the mutex */
            pthread_mutex_unlock (&thread_count_mutex);

        }
        else
        {
            /** Unlock the mutex */
            pthread_mutex_unlock (&thread_count_mutex);

            sem_wait (&max_threads_sem);
        }
    }
}



static void *
_guardian_scheduler_thread_run (void *__arg)
{

    pthread_mutex_lock (&thread_busy_mutex);
    pthread_mutex_unlock (&thread_busy_mutex);

    printf(".1\n");
    pthread_mutex_lock (&thread_count_mutex);
    printf(".2\n");

    if (thread_count >= (THREADPOOL_SIZE-1))
    {
        /** decrease the running thread-count */
        thread_count--;
        pthread_mutex_unlock (&thread_count_mutex);

        sem_post (&max_threads_sem);
    }
    else
    {
        /** decrease the running thread-count */
        thread_count--;
        pthread_mutex_unlock (&thread_count_mutex);
    }

    pthread_exit (NULL);
}
