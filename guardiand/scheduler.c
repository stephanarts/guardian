/*-
 * Copyright (c) 2012-2014 Stephan Arts. All Rights Reserved.
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

#include <unistd.h>

#include <string.h>

#include <sys/types.h>

#include <zmq.h>

#include <pthread.h>
#include <errno.h>

#include <time.h>

#include <semaphore.h>

#include <libguardian/libguardian.h>

#include "worker.h"
#include "db.h"

/** Define 10 Second interval */
#define INTERVAL 1

static int n_sources = 0;
static GuardianSource **sources = NULL;
static char *source_state = NULL;
void *_ctx = NULL;

#define         BUFFER_LEN      1024
char            buffer[BUFFER_LEN];

void
guardian_scheduler_main ( void *ctx, int n_workers )
{
    int i = 0;
    int n_sleeping = 0;
    int no_linger = 0;
    int ret = 0;
    void *plugins;
    void *controller;
    void *data_processor;
    void *agent;
    GuardianSource *source = NULL;

    pthread_t *workers = NULL;

    if (_ctx != NULL) {
        return;
    }

    _ctx = ctx;

    plugins    = zmq_socket(ctx, ZMQ_REP);
    controller = zmq_socket(ctx, ZMQ_ROUTER);
    data_processor = zmq_socket(ctx, ZMQ_REP);
    agent = zmq_socket(ctx, ZMQ_ROUTER);

    zmq_bind(plugins,    "inproc://workers");
    zmq_bind(controller, "inproc://controller");
    zmq_bind(data_processor, "inproc://data-processor");
    zmq_bind(agent, "tcp://127.0.0.1:5678");

    workers = guardian_new (sizeof(pthread_t), n_workers);

    for (i = 0; i < n_workers; ++i)
    {
        guardian_worker_thread_new (
            &workers[i],
            _ctx);
    }

    /**
     * Enter the main loop, schedules jobs in the queue
     *
     * NOTE:
     * Hmm, the queue is still missing
     */
    while (1)
    {
        char msg [256];
        zmq_pollitem_t items [] = {
            { plugins, 0, ZMQ_POLLIN, 0 },
            { controller, 0, ZMQ_POLLIN, 0 },
            { data_processor, 0, ZMQ_POLLIN, 0 },
            { agent, 0, ZMQ_POLLIN, 0 },
        };
        zmq_poll (items, 3, -1);

        /* Scheduling of worker threads */
        if (items [0].revents & ZMQ_POLLIN) {
            int size = zmq_recv (plugins, msg, 255, 0);
            if (size != -1) {
                // Process task
                if(!strncmp(msg, "GET-COMMAND", 11)) {
                    source = NULL;
                    n_sleeping = 0;
                    for(i = 0; i < n_sources; ++i) {
                        if (source_state[i] == 'S') {
                            n_sleeping++;
                        }
                        if (source_state[i] == 'I') {
                            source = sources[i];
                            source_state[i] = 'R';
                            break;
                        }
                    }

                    /* If no source available, wait */
                    if (source == NULL) {
                        /* If enough sources are sleeping,
                         * mark them as idle so they can be
                         * scheduled again.
                         */
                        if (n_sleeping == n_sources) {
                            for(i = 0; i < n_sources; ++i) {
                                source_state[i] = 'I';
                            }
                        }

                        snprintf(msg, 255, "WAIT[%d]%n", INTERVAL, &size);
                        zmq_send(plugins, msg, size, 0);
                    } else {
                        guardian_log_debug("Worker Processing Source[%d]", i);
                        snprintf(msg, 255, "PROCESS[%p]%n", source, &size);
                        zmq_send(plugins, msg, size, 0);
                    }
                    continue;
                }
                /* Check if it is a message indicating
                 * a worker thread is finished processing.
                 */
                ret = sscanf(msg, "FINISH[%p]", &source);
                if (ret == 1) {
                    guardian_log_debug("Worker Finished");
                    /* Mark the source as sleeping so it
                     * won't be rescheduled immediately.
                     */
                    for(i = 0; i < n_sources; ++i) {
                        if(sources[i] == source) {
                            source_state[i] = 'S';
                            break;
                        }
                    }
                    zmq_send(plugins, "-", 1, 0);
                    continue;
                } 
                guardian_log_debug("Send 'a': '%s'", msg);
                zmq_send(plugins, "a", 1, 0);
            }
        }

        /* Listen to Controller socket for STOP message */
        if (items [1].revents & ZMQ_POLLIN) {
            int size = zmq_recv (controller, msg, 255, 0);
            if (size != -1) {
                if(strncmp(msg, "STOP", 4)) {
                    guardian_log_debug("Terminating main loop");

                    /** TODO:
                     * Use another mechanism to properly end threads.
                     */

                    for (i = 0; i < n_workers; ++i)
                    {
                        pthread_cancel(workers[i]);
                        pthread_join(workers[i], NULL);
                    }
                    break;
                }
            }
        }

        /* Put data in a database */
        if (items [2].revents & ZMQ_POLLIN) {

            while (1) {
                /* Every message-part is a log-entry */
                zmq_msg_t message;
                zmq_msg_init (&message);
                zmq_msg_recv (&message, data_processor, 0);

                //guardian_db_insert_entry();

                //t = zmq_msg_size(&message);

                zmq_msg_close(&message); 

                if (!zmq_msg_more (&message)) {
                    break;
                }
            }

            zmq_send(data_processor, "0", 1, 0);
        }

        /* Put data in a database */
        if (items [3].revents & ZMQ_POLLIN) {
            zmq_msg_t message;
            zmq_msg_init (&message);
            zmq_msg_recv (&message, agent, 0);

            zmq_msg_close(&message); 

            zmq_send(agent, "0", 1, 0);
        }
    }

    zmq_setsockopt(controller, ZMQ_LINGER, &no_linger, sizeof(no_linger));
    zmq_close (controller);
    zmq_setsockopt(plugins, ZMQ_LINGER, &no_linger, sizeof(no_linger));
    zmq_close (plugins);

    _ctx = NULL;
}

void
guardian_scheduler_main_quit ( )
{
    void *socket = zmq_socket(_ctx, ZMQ_REQ);
    int ret = 0;

    guardian_log_debug("Terminating %s", PACKAGE_NAME);

    zmq_connect(socket, "inproc://controller");

    ret = zmq_send(socket, "STOP", 4, 0);
    if (ret == -1)
    {
        guardian_log_error("Failed to send termination message");
    }

    zmq_close(socket);
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
    char *_source_state;
    int i;

    if (source == NULL) {
        guardian_log_error("Can not schedule NULL source\n");
        return;
    }

    /** If no sources are defined */
    if (sources == NULL)
    {
        sources = guardian_new(sizeof(GuardianSource *), 2);
        sources[0] = source;
        sources[1] = NULL;
        source_state = guardian_new(sizeof(char), 2);
        source_state[0] = 'I';
        source_state[1] = '\0';
        n_sources = 1;
        return;
    }

    /** Increase the source list */
    _sources = guardian_new(sizeof(GuardianSource *) * (n_sources + 2), 1);
    _source_state = guardian_new(sizeof(char) * (n_sources + 2), 1);

    /** Copy all sources to the new list */
    for(i = 0; i < n_sources; ++i)
    {
        _sources[i] = sources[i];
        _source_state[i] = source_state[i];
    }
    _sources[i] = source;
    _source_state[i] = 'I';

    /** Free the old list */
    free (sources);
    free (source_state);

    /** And replace it with the new one */
    sources = _sources;
    source_state = _source_state;

    n_sources++;
}

