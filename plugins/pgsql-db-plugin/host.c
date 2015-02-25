/*-
 * Copyright (c) 2014 Stephan Arts. All Rights Reserved.
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

#include <openssl/sha.h>

#include <string.h>

#include <pcre.h>

#include <errno.h>

#include <sys/stat.h>

#include <libguardian/libguardian.h>

#include "plugin.h"
#include "db.h"
#include "host.h"
#include "ns.h"

#define HOSTNAME_MAXLEN 255
#define HOSTS_CACHESIZE 10

struct _Host
{
    int host_id;
    char name[HOSTNAME_MAXLEN+1];
};

Host _hosts_cache[HOSTS_CACHESIZE];
Host *_i_hosts_cache_name[HOSTS_CACHESIZE];
int  _n_hosts_cache = 0;

/**
 * _pgsql_host_get
 *
 * @param name
 * @param host_ptr
 * @param error
 */
int
_pgsql_host_get (
        const char *name,
        void **host_ptr,
        GuardianError **error)
{
    return 0;
}

/**
 * _pgsql_host_add
 *
 * @param name
 * @param error
 */
int
_pgsql_host_add (
        const char *host,
        GuardianError **error)
{
    return 0;
}

/**
 * _pgsql_host_getid
 *
 * @param host
 * @param error
 */
int
_pgsql_host_getid (
        Host *host,
        GuardianError **error)
{
    return host->host_id;
}
