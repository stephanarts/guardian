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

#include <time.h>

#include <libguardian/libguardian.h>

/** Define 5 Second interval */
#define INTERVAL 2

static int n_sources = 0;
static GuardianSource **sources = NULL;

void
guardian_scheduler_main ( void )
{
    struct timespec s_t;
    struct timespec e_t;
    struct timespec sl_t;
    int i;

    GuardianSource **_source_ptr;

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
