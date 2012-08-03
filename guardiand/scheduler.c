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
#include <errno.h>
#include <semaphore.h>

#include <time.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

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
sem_t           max_threads_sem;

/**
 * Any thread-job is first pushed to a queue.
 */
#define         QUEUE_SIZE      10
sem_t           queue_size_sem;

static int      thread_count = 0;

#define         SOCK_PATH "/tmp/guardian.sock"
#define         BUFFER_LEN      1024
char            buffer[BUFFER_LEN];

static int      main_loop_running = 0;


static void *
_guardian_scheduler_thread_run (void *__arg);

static void *
guardian_scheduler_concept (void *__arg);

void
guardian_scheduler_main ( void )
{
    struct timespec timeout;
    int fd;
    int n_fds = 1;
    fd_set r_fds;
    pthread_t thread;

    int s, r_s, len;
    struct sockaddr_un local;
    struct sockaddr_un r_addr;
    socklen_t r_addr_size;


    /** If the main-loop is already running, return */
    if (main_loop_running == 1)
    {
        return;
    }

    main_loop_running = 1;

    /** Set the timeout on 10 seconds */
    timeout.tv_sec = 10;
    timeout.tv_nsec = 0;

    FD_ZERO (&r_fds);

    if ((s = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    r_addr_size = sizeof(struct sockaddr_un);

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_PATH);
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(s, (struct sockaddr *)&local, len) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(s, 5) == -1) {
        perror("listen");
        exit(1);
    }

    FD_SET (s, &r_fds);

    /** 
     * Create semaphore used for guarding the maximum
     * number of simultanious running threads.
     */
    sem_init (
            &max_threads_sem,
            0,
            THREADPOOL_SIZE );

    /** 
     * Create semaphore used for guarding the event-queue.
     */
    sem_init (
            &queue_size_sem,
            0,
            0 );
    /*
     * Spawn a new thread, this thread will be responsible for
     * getting some work done.
     */
    pthread_create (
            &thread,
            NULL,
            guardian_scheduler_concept,
            NULL);

    /**
     * Enter the main loop, schedules jobs in the queue
     *
     * NOTE:
     * Hmm, the queue is still missing
     */
    while (1)
    {
        fd = pselect (
                n_fds,      /** Number of file-descriptors */
                &r_fds,     /** Read FD_SET         */
                NULL,       /** Write FD_SET        */
                NULL,       /** Except FD_SET       */
                &timeout,   /** Timeout timespec    */
                NULL );     /** Signal sigset_t     */
        switch (fd)
        {
            /** ERROR */
            case -1: 
                break;
            /** Timeout */
            case 0:
                printf(".\n");
                break;
            /** Oooh, we have data */
            /** 
             * TODO:
             * Think of a way to prevent a DoS on the update mechanism,
             * flooding the queue with commands may never prevent the
             * program from updating the datasources.
             */
            default:
                /** If we have data on the listening socket, accept */
                if (s == fd)
                {
                    r_s = accept ( s, &r_addr, &r_addr_size);
                    FD_SET (r_s, &r_fds);
                }
                else
                {
                    /** Read data */
                    recv (fd, buffer, BUFFER_LEN, 0);
                }
                break;
        }

        /** If we return from the loop here, we exit */
        if (main_loop_running == 0)
        {
            /** Close the socket */
            close (s);

            /** Remove the unix socket file */
            unlink (SOCK_PATH);

            /**
             * Cancel the 'scheduler' thread, it is probably stuck 
             * in a semaphore.
             */
            pthread_cancel (thread);

            /** Exit this thread */
            pthread_exit (NULL);
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

    /** If no sources are defined */
    if (sources == NULL)
    {
        sources = malloc (sizeof(GuardianSource *) * 2);
        sources[0] = source;
        sources[1] = NULL;
        n_sources = 1;
        return;
    }

    /** Increase the source list */
    _sources = malloc (sizeof(GuardianSource *) * n_sources + 2);

    /** Copy all sources to the new list */
    for(i = 0; i < n_sources; ++i)
    {
        _sources[i] = sources[i];
    }
    _sources[i] = source;

    /** Free the old list */
    free (sources);

    /** And replace it with the new one */
    sources = _sources;

    n_sources++;
}

static void *
guardian_scheduler_concept (void *__arg)
{
    int ret;
    pthread_t      thread;
    pthread_attr_t attr;

    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    while (1)
    {
        /**
         * If the queue is empty, wait for a new job to be queued.
         */
        sem_wait (&queue_size_sem);

        /** If we return from the loop here, we exit */
        if (main_loop_running == 0)
        {
            pthread_exit (NULL);
        }

        /** Spawn a new worker thread */
        ret = pthread_create (
            &thread,
            &attr,
            _guardian_scheduler_thread_run,
            NULL);

        if (ret == 0)
        {
            /** Ooh, we have a thread */
            thread_count++;
        }
        else
        {
            /** Stuff went wrong */
            switch (ret)
            {
                case EAGAIN:
                    /**
                     * Now we have a problem, could be memory,
                     * limits or other constraints
                     */
                    guardian_log_critical (
                            "CRITICAL:  %s",
                            "No resources to create thread\n");
                    break;
                case EPERM:
                case EINVAL:
                default:
                    /** We don't really care why it went wrong here */
                    guardian_log_critical (
                            "CRITICAL:  %s",
                            "Could not create thread; unknown error\n");
                    break;
            }

            /*
             * We could not create a thread, but the thread-pool is
             * not full. Push to the semaphore to prevent a deadlock.
             *
             * As a result, this thread will not dead-lock,
             * but it will jump to 100% CPU and keep the system busy.
             *
             * Might not be the brightest idea ;-)
             */
            sem_post (&max_threads_sem);
        }

        /**
         * If the maximum number of threads is running,
         * wait for the first one to close.
         */
        sem_wait (&max_threads_sem);
    }

    /* This, we never reach */
    pthread_exit (NULL);
}



static void *
_guardian_scheduler_thread_run (void *__arg)
{
    /* BEGIN THREAD BODY, THE REAL STUFF */

//    sleep (1);

    /* END THREAD BODY, THE REAL STUFF */

    if (sem_post (&max_threads_sem) == -1)
    {
        //printf("SEM POST FAILED\n");
    }

    pthread_exit (NULL);
}


void
guardian_scheduler_queue_push ()
{

    sem_post (&queue_size_sem);
}

void
guardian_scheduler_main_quit ( void )
{
    /*pthread_mutex_lock ();*/
    main_loop_running = 0;
    /*pthread_mutex_unlock ();*/
}
