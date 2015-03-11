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
#include "host.h"
#include "ns.h"

static sqlite3 *_sqlite3_db = NULL;

typedef struct {
    char name[128];
    char value[128];
} prop;

enum {
    PROP_DB_PATH = 0,
    PROP_COUNT
};

static prop props[] = {
    {"db_path", "/tmp/guardian.db" }
};

int
_sqlite3_db_setprop (
        const char *key,
        const char *value)
{
    return 0;
}

int
_sqlite3_db_getprop (
        const char *key,
        char **value)
{
    return 0;
}

int
_sqlite3_db_listprop (
        char **keys[])
{
    int i;
    char **_props = malloc ((sizeof (char *)) * PROP_COUNT);

    for (i = 0; i < PROP_COUNT; ++i) {
        _props[i] = props[i].name;
    }

    *keys = _props;

    return PROP_COUNT;
}

int
_sqlite3_db_connect (
        GuardianError **error)
{
    int ret = 0;
    if (_sqlite3_db == NULL)
    {
        fprintf(stderr, "OPEN DB\n");
        ret = sqlite3_open (
                props[PROP_DB_PATH].value,
                &_sqlite3_db);
        if (ret != SQLITE_OK)
        {
            fprintf(stderr, "FAIL\n");
            return 1;
        }
    }

    return 0;
}

int
_sqlite3_db_disconnect (
        GuardianError **error)
{
    fprintf(stderr, "CLOSE DB\n");
    sqlite3_close (_sqlite3_db);
    return 0;
}

sqlite3 *
_sqlite3_db_get (void)
{
    return _sqlite3_db;
}
