/*-
 * Copyright (c) 2015 Stephan Arts. All Rights Reserved.
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
#include "perm.h"

int
_sqlite3_perm_host_set (
        const char *role,
        const char *host,
        unsigned int perm,
        GuardianError **error)
{
/*
    char query[128];
    sqlite3 *db = NULL;
    sqlite3_stmt *handle = NULL;
    int ret;
    const char *errmsg;
    int host_id = _sqlite3_host_getid (object,
            error);

    if (host_id < 0)
    {
        return -1;
    }

    db = _sqlite3_db_get();

    sqlite3_snprintf (
            128,
            query,
            "SELECT id FROM 'HOST_PERM' "
            "WHERE host_id = %d "
            "AND role_id = (SELECT id FROM ROLES WHERE name IS '%q')",
            host_id,
            role);

    ret = sqlite3_prepare_v2 (
            db,
            query,
            -1,
            &handle,
            NULL);
    if (ret != SQLITE_OK)
    {
        if (error != NULL) {
            errmsg = sqlite3_errmsg (db);
            *error = guardian_error_new (
                    "%s",
                    errmsg);
        }
        return -1;
    }
*/
    return 0;
}

int
_sqlite3_perm_host_get (
        const char *role,
        const char *host,
        unsigned int *perm,
        GuardianError **error)
{
    return 0;
}

int
_sqlite3_perm_host_check(
        const char *role,
        const char *host,
        unsigned int perm,
        GuardianError **error)
{
    return 0;
}
