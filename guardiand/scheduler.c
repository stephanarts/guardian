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

#include <string.h>

#include <sys/types.h>

#include <zmq.h>

#include <pthread.h>
#include <errno.h>

#include <time.h>

#include <libguardian/libguardian.h>

/** Define 5 Second interval */
#define INTERVAL 2
static int n_sources = 0;
static GuardianSource **sources = NULL;
void *_ctx = NULL;

#define         BUFFER_LEN      1024
char            buffer[BUFFER_LEN];

void
guardian_scheduler_main ( void *ctx )
{
    int no_linger = 0;
    void *plugins;
    void *controller;

    if (_ctx != NULL) {
        return;
    }

    _ctx = ctx;

    plugins    = zmq_socket(ctx, ZMQ_ROUTER);
    controller = zmq_socket(ctx, ZMQ_ROUTER);

    zmq_bind(plugins,    "inproc://plugins");
    zmq_bind(controller, "inproc://controller");

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
            { plugins , 0, ZMQ_POLLIN, 0 },
            { controller, 0, ZMQ_POLLIN, 0 }
        };
        zmq_poll (items, 2, -1);

        if (items [0].revents & ZMQ_POLLIN) {
            int size = zmq_recv (plugins, msg, 255, 0);
            if (size != -1) {
                // Process task
            }
        }

        if (items [1].revents & ZMQ_POLLIN) {
            int size = zmq_recv (controller, msg, 255, 0);
            if (size != -1) {
                if(strncmp(msg, "42", 2)) {
                    guardian_log_debug("Terminating main loop");
                    break;
                }
            }
        }
    }
    zmq_setsockopt(controller, ZMQ_LINGER, &no_linger, sizeof(no_linger));
    zmq_close (controller);
    zmq_setsockopt(plugins, ZMQ_LINGER, &no_linger, sizeof(no_linger));
    zmq_close (plugins);

    _ctx = NULL;
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
        sources = guardian_new(sizeof(GuardianSource *), 2);
        sources[0] = source;
        sources[1] = NULL;
        n_sources = 1;
        return;
    }

    /** Increase the source list */
    _sources = guardian_new(sizeof(GuardianSource *) * (n_sources + 2), 1);

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

void
guardian_scheduler_main_quit ( )
{
    void *socket = zmq_socket(_ctx, ZMQ_REQ);
    int ret = 0;

    guardian_log_debug("Terminating %s", PACKAGE_NAME);
    
    zmq_connect(socket, "inproc://controller");

    ret = zmq_send(socket, "42", 2, 0);
    if (ret == -1)
    {
        guardian_log_error("Failed to send termination message");
    }

    zmq_close(socket);
}
