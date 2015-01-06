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

static void *_ctx = NULL;

static void
msg_free (void *data, void *hint)
{
    free (data);
}

static void *
_guardian_worker_thread (void *arg)
{
    char    msg[256];
    void   *socket;
    void   *d_socket;
    int     ret = 0;
    int     timeout = 0;
    zmq_msg_t n_entries_message;

    socket = zmq_socket (_ctx, ZMQ_REQ);
    d_socket = zmq_socket (_ctx, ZMQ_REQ);
    zmq_connect (socket, "inproc://workers");
    zmq_connect (d_socket, "inproc://data-processor");
    while (1)
    {
        //printf (".");
        zmq_send (socket, "GET-COMMAND\n\0", 13, 0);
        zmq_recv (socket, msg, 255, 100);

        //guardian_log_info ("..'%s'\n", msg);
        ret = sscanf (msg, "WAIT[%d]", &timeout);
        if (ret == 1)
        {
            guardian_log_info ("WORKER SLEEP\n");
            sleep (timeout);
            continue;
        }
        ret = sscanf (msg, "PROCESS");
        if (ret == 1)
        {
            void   *data;
            data = malloc (sizeof (int));
            *((int *)data) = (int)0xA;
            zmq_msg_init_data (
                    &n_entries_message,
                    data,
                    sizeof (int),
                    msg_free,
                    NULL);
            zmq_msg_send (&n_entries_message, d_socket, 0);

            zmq_recv (d_socket, msg, 255, 100);

            sprintf (msg, "FINISH%n", &ret);
            zmq_send (socket, msg, ret, 0);
            zmq_recv (socket, msg, 255, 100);

            guardian_log_info ("PROCESS SOURCE DONE\n");
        }
    }
    pthread_exit (NULL);
}


int
guardian_worker_thread_new (
        pthread_t * thread,
        void *ctx)
{
    if (_ctx == NULL)
        _ctx = ctx;
    pthread_create (
            thread,
            NULL,
            _guardian_worker_thread,
            NULL);

    return 0;
}
