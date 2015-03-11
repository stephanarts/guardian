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

#include <libgen.h>

#include <string.h>

#include <pcre.h>

#include <errno.h>

#include <sys/stat.h>

#include <sqlite3.h>

#include <libguardian/libguardian.h>

#include "plugin.h"
#include "host.h"
#include "ns.h"
#include "db.h"

static sqlite3 *_sqlite3_db = NULL;

typedef struct {
    char name[128];
    char value[128];
} prop;

enum {
    PROP_DB_PATH = 0,
    PROP_DB_SCHEMA,
    PROP_SQLITE3_CMD,
    PROP_COUNT
};

static prop props[] = {
    {"db_path", "/tmp/guardian.db" },
    {"db_schema", SCHEMADIR"/sqlite3.schema" },
    {"sqlite3_cmd", "sqlite3" }
};

int
_sqlite3_db_setprop (
        const char *name,
        const char *value)
{
    int i = 0;
    for (; i < PROP_COUNT; ++i) {
        if (strcmp(props[i].name, name) == 0) {
            strncpy(props[i].value, value, 128);
            return 0;
        }
    }
    return 1;
}

int
_sqlite3_db_getprop (
        const char *name,
        char **value)
{
    int i = 0;
    for (; i < PROP_COUNT; ++i) {
        if (strcmp(props[i].name, name) == 0) {
            (*value) = (char *)(&props[i].value);
            return 0;
        }
    }
    return 1;
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
    struct stat _c_stat;

    if (_sqlite3_db == NULL)
    {
        ret = stat(props[PROP_DB_PATH].value, &_c_stat);
        if (ret != 0) {
            /* File does not exists... we don't want sqlite3_open to create an empty one. */
            if (error != NULL) {
                *error = guardian_error_new (
                        "Cannot open sqlite-database at '%s', file missing.\n",
                        props[PROP_DB_PATH].value);
            }
            return 1;
        }

        ret = sqlite3_open (
                props[PROP_DB_PATH].value,
                &_sqlite3_db);
        if (ret != SQLITE_OK)
        {
            if (error != NULL) {
                *error = guardian_error_new (
                        "Open Failed\n");
            }
            return 1;
        }
    }

    return 0;
}

int
_sqlite3_db_disconnect (
        GuardianError **error)
{
    sqlite3_close (_sqlite3_db);
    return 0;
}

int
_sqlite3_db_init (
        GuardianError **error)
{
    char *path        = NULL;
    char *schema      = NULL;
    char *sqlite3_cmd = NULL;

    char command[256];
    struct stat _c_stat;

    _sqlite3_db_getprop("db_path",   &path);
    _sqlite3_db_getprop("db_schema", &schema);
    _sqlite3_db_getprop("sqlite3_cmd", &sqlite3_cmd);

    int ret = stat(path, &_c_stat);
    if (ret == 0) {
        /* File exists... we are creating it... E_OOPS */
        if (error != NULL) {
            *error = guardian_error_new (
                    "Cannot initialize sqlite-database at '%s', file exists.\n",
                    path);
        }
        return 1;
    }
    if (errno != ENOENT) {
        if (error != NULL) {
            *error = guardian_error_new (
                    "Cannot initialize sqlite-database at '%s'.\n",
                    path);
        }
        return 1;
    }

    ret = stat(dirname(path), &_c_stat);
    if (ret != 0) {
        /* Directory is missing... we can't create the file. */
        if (error != NULL) {
            *error = guardian_error_new (
                    "Directory '%s' does not exist. Cannot create sqlite-database.\n",
                    dirname(path));
        }
        return 1;
    }

    ret = stat(schema, &_c_stat);
    if (ret != 0) {
        /* Schema is missing... we can't create the file. */
        if (error != NULL) {
            *error = guardian_error_new (
                    "Schema '%s' does not exist. Cannot create sqlite-database.\n",
                    schema);
        }
        return 1;
    }

    sprintf(command, "cat %s | %s %s",
            schema,
            sqlite3_cmd,
            path);

    ret = system(command);
    if (ret != 0) {
        if (error != NULL) {
            *error = guardian_error_new (
                    "Creating database failed.\n");
        }
        return 1;
    }

    return 0;
}

sqlite3 *
_sqlite3_db_get (void)
{
    return _sqlite3_db;
}
