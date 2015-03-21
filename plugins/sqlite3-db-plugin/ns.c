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

#include <sqlite3.h>

#include <libguardian/libguardian.h>

#include "plugin.h"
#include "db.h"
#include "host.h"
#include "ns.h"

#define NAMESPACE_MAXLEN 63

struct _Ns
{
    int ns_id;
    char name[NAMESPACE_MAXLEN+1];
};

Ns _nss[10];
Ns _ns;

int 
_sqlite3_ns_add (
        const char *name,
        void *host_ptr,
        GuardianError **error)
{
    char query[128];
    sqlite3_stmt *handle = NULL;
    int ret;
    const char *errmsg;
    int host_id;
    GuardianError *call_error = NULL;

    sqlite3 *db = _sqlite3_db_get();

    host_id = _sqlite3_host_getid ( host_ptr, &call_error);
    if (host_id == -1)
    {
        *error = call_error;
        return -1;
    }

    sqlite3_snprintf (
            128,
            query,
            "INSERT INTO 'NAMESPACE'(host_id,name) "
            "VALUES(%d,'%q');",
            host_id,
            name);

    ret = sqlite3_prepare_v2 (
            db,
            query,
            -1,
            &handle,
            NULL);
    if (ret != SQLITE_OK)
    {
        errmsg = sqlite3_errmsg (db);
        *error = guardian_error_new (
                "%s",
                errmsg);
        return -1;
    }

    do
    {
        ret = sqlite3_step (handle);
    } while (ret == SQLITE_BUSY);

    switch (ret)
    {
        case SQLITE_INTERRUPT:
        case SQLITE_SCHEMA:
        case SQLITE_CORRUPT:
            errmsg = sqlite3_errmsg (db);
            *error = guardian_error_new (
                    "%s",
                    errmsg);
            sqlite3_finalize(handle);
            return -1;
            break;
        case SQLITE_DONE:
            break;
        default:
            errmsg = sqlite3_errmsg (db);
            *error = guardian_error_new (
                    "%s",
                    errmsg);
            sqlite3_finalize(handle);
            return -1;
            break;
    }

    sqlite3_finalize(handle);

    return 0;
}

int
_sqlite3_ns_list (
        char **nss,
        int *n_nss,
        void *host_ptr,
        GuardianError **error)
{
    char query[128];
    sqlite3_stmt *handle = NULL;
    int ret;
    const char *errmsg;
    int host_id;
    GuardianError *call_error = NULL;

    sqlite3 *db = _sqlite3_db_get();

    host_id = _sqlite3_host_getid ( host_ptr, &call_error);
    if (host_id == -1)
    {
        return -1;
    }

    sqlite3_snprintf (
            128,
            query,
            "SELECT name FROM 'NAMESPACE' "
            "WHERE host_id = %d "
            "ORDER BY NAME DESC;",
            host_id);

    ret = sqlite3_prepare_v2 (
            db,
            query,
            -1,
            &handle,
            NULL);
    

    ret = sqlite3_step (handle);
    for (;;)
    {
        switch (ret)
        {
            case SQLITE_INTERRUPT:
            case SQLITE_SCHEMA:
            case SQLITE_CORRUPT:
                errmsg = sqlite3_errmsg (db);
                *error = guardian_error_new (
                        "%s",
                        errmsg);
                sqlite3_finalize(handle);
                return -1;
                break;
            default:
                errmsg = sqlite3_errmsg (db);
                *error = guardian_error_new (
                        "%s",
                        errmsg);
                sqlite3_finalize(handle);
                return -1;
                break;
            case SQLITE_BUSY:
                ret = sqlite3_step(handle);
                break;
            case SQLITE_ROW:
                printf("> %s\n",
                    sqlite3_column_text (handle, 0));
                ret = sqlite3_step(handle);
                break;
            case SQLITE_DONE:
                sqlite3_finalize(handle);
                return 0;
                break;
        }

    }

    return 0;
}

int
_sqlite3_ns_get (
        const char *name,
        void *host_ptr,
        void **ns_ptr,
        GuardianError **error)
{
    char query[128];
    sqlite3_stmt *handle = NULL;
    int ret;
    const char *errmsg;
    int host_id;
    int ns_id;

    GuardianError *call_error = NULL;

    sqlite3 *db = _sqlite3_db_get();

    host_id = _sqlite3_host_getid ((Host *)host_ptr, &call_error);
    if (host_id == -1)
    {
        *error = call_error;
        return -1;
    }

    sqlite3_snprintf (
            128,
            query,
            "SELECT id FROM 'NAMESPACE' WHERE name='%q' AND host_id=%d;",
            name,
            host_id);

    ret = sqlite3_prepare_v2 (
            db,
            query,
            -1,
            &handle,
            NULL);
    if (ret != SQLITE_OK)
    {
        errmsg = sqlite3_errmsg (db);
        *error = guardian_error_new (
                "%s",
                errmsg);
        return -1;
    }

    do
    {
        ret = sqlite3_step (handle);
    } while (ret == SQLITE_BUSY);

    switch (ret)
    {
        case SQLITE_INTERRUPT:
        case SQLITE_SCHEMA:
        case SQLITE_CORRUPT:
        default:
            errmsg = sqlite3_errmsg (db);
            *error = guardian_error_new (
                    "%s",
                    errmsg);
            sqlite3_finalize(handle);
            return -1;
            break;
        case SQLITE_ROW:
            ns_id = sqlite3_column_int (handle, 0);
            break;
        case SQLITE_DONE:
            *error = guardian_error_new (
                    "Namespace '%s' for host '%s' not found.",
                    name,
                    "unknown");
            sqlite3_finalize(handle);
            return -1;
            break;
    }

    ret = sqlite3_step (handle);
    switch (ret)
    {
        case SQLITE_INTERRUPT:
        case SQLITE_SCHEMA:
        case SQLITE_CORRUPT:
            errmsg = sqlite3_errmsg (db);
            *error = guardian_error_new (
                    "%s",
                    errmsg);
            sqlite3_finalize(handle);
            return -1;
            break;
        case SQLITE_ROW:
            *error = guardian_error_new (
                    "Multiple entries of namespace '%s'.\n",
                    name);
            sqlite3_finalize(handle);
            return -1;
            break;
        case SQLITE_DONE:
            sqlite3_finalize(handle);
            break;
    }

    strncpy(_ns.name, name, NAMESPACE_MAXLEN);
    _ns.ns_id = ns_id;
    (*ns_ptr) = &_ns;

    return 0;
}

int
_sqlite3_ns_getid (
        Ns *ns,
        GuardianError **error)
{
    return ns->ns_id;
}
