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

static sqlite3 *_sqlite3_db = NULL;

static int
_sqlite3_get_hostid (
        const char *name,
        GuardianError **error);

static void
_sqlite3_db_set (
        const char *key,
        const char *value);

static void
_sqlite3_db_connect (
        void);

static void
_sqlite3_db_disconnect (
        void);

static void
_sqlite3_add_role (
        const char *name,
        GuardianError **error);

static int
_sqlite3_ns_add (
        const char *host,
        const char *name,
        GuardianError **error);

static int 
_sqlite3_ns_list (
        const char *host,
        char **nss,
        int *len,
        GuardianError **error);

GuardianPlugin *
guardian_plugin_init ()
{
    const char *errors = NULL;
    int     err_offset;
    GuardianPluginDB *plugin;

    guardian_log_info ("Initialise SQLite3 plugin");

    plugin = guardian_new (sizeof (GuardianPluginDB), 1);

    plugin->schema_version = 1;

    /* Copy the name */
    strncpy(plugin->db_name, "sqlite3", 20);

    /* DB */
    plugin->db.set        = _sqlite3_db_set;
    plugin->db.connect    = _sqlite3_db_connect;
    plugin->db.disconnect = _sqlite3_db_disconnect;

    /* Roles */
    plugin->roles.add = _sqlite3_add_role;

    /* Namespace */
    plugin->ns.add = _sqlite3_ns_add;
    plugin->ns.list = _sqlite3_ns_list;

    /* ACL */
    //plugin->acl.add = _sqlite3_acl_add;

    return (GuardianPlugin *)plugin;
}

static void
_sqlite3_db_set (
        const char *key,
        const char *value)
{

}

static void
_sqlite3_db_connect (
        void)
{
    if (_sqlite3_db == NULL)
    {
        fprintf(stderr, "OPEN DB");
        sqlite3_open (
                "/tmp/guardian.db",
                &_sqlite3_db);
    }
}

static void
_sqlite3_db_disconnect (
        void)
{
    sqlite3_close (_sqlite3_db);
}

static void
_sqlite3_add_role (
        const char *name,
        GuardianError **error)
{

}

static int 
_sqlite3_ns_add (
        const char *host,
        const char *name,
        GuardianError **error)
{
    char query[128];
    sqlite3_stmt *handle = NULL;
    int ret;
    const char *errmsg;
    int host_id;
    GuardianError *call_error = NULL;

    host_id = _sqlite3_get_hostid ( host, &call_error);
    if (host_id == -1)
    {
        return -1;
    }

    snprintf (
            query,
            128,
            "INSERT INTO 'NAMESPACE'(host_id,name) "
            "VALUES(%d,\"%s\");",
            host_id,
            name);

    ret = sqlite3_prepare_v2 (
            _sqlite3_db,
            query,
            -1,
            &handle,
            NULL);
    do
    {
        ret = sqlite3_step (handle);
    } while (ret == SQLITE_BUSY);

    switch (ret)
    {
        case SQLITE_INTERRUPT:
        case SQLITE_SCHEMA:
        case SQLITE_CORRUPT:
            errmsg = sqlite3_errmsg (_sqlite3_db);
            *error = guardian_error_new (
                    "%s",
                    errmsg);
            sqlite3_finalize(handle);
            return -1;
            break;
        case SQLITE_DONE:
            break;
        default:
            errmsg = sqlite3_errmsg (_sqlite3_db);
            *error = guardian_error_new (
                    "%s",
                    errmsg);
            sqlite3_finalize(handle);
            return -1;
            break;
    }

    return 0;
}

static int
_sqlite3_ns_list (
        const char *host,
        char **nss,
        int *len,
        GuardianError **error)
{
    char query[128];
    sqlite3_stmt *handle = NULL;
    int ret;
    const char *errmsg;
    int host_id;
    GuardianError *call_error = NULL;

    host_id = _sqlite3_get_hostid ( host, &call_error);
    if (host_id == -1)
    {
        return -1;
    }

    snprintf (
            query,
            128,
            "SELECT name FROM 'NAMESPACE' "
            "WHERE host_id = %d "
            "ORDER BY NAME DESC;",
            host_id);

    ret = sqlite3_prepare_v2 (
            _sqlite3_db,
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
                errmsg = sqlite3_errmsg (_sqlite3_db);
                *error = guardian_error_new (
                        "%s",
                        errmsg);
                sqlite3_finalize(handle);
                return -1;
                break;
            default:
                errmsg = sqlite3_errmsg (_sqlite3_db);
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

static int
_sqlite3_get_hostid (
        const char *host,
        GuardianError **error)
{
    char query[128];
    sqlite3_stmt *handle = NULL;
    int ret;
    const char *errmsg;
    int host_id;

    snprintf (
            query,
            128,
            "SELECT id FROM 'HOSTS' WHERE name='%s';",
            host);

    ret = sqlite3_prepare_v2 (
            _sqlite3_db,
            query,
            -1,
            &handle,
            NULL);

    do
    {
        ret = sqlite3_step (handle);
    } while (ret == SQLITE_BUSY);

    switch (ret)
    {
        case SQLITE_INTERRUPT:
        case SQLITE_SCHEMA:
        case SQLITE_CORRUPT:
            errmsg = sqlite3_errmsg (_sqlite3_db);
            *error = guardian_error_new (
                    "%s",
                    errmsg);
            sqlite3_finalize(handle);
            return -1;
            break;
        case SQLITE_ROW:
            host_id = sqlite3_column_int (handle, 0);
            break;
        case SQLITE_DONE:
            *error = guardian_error_new (
                    "Host '%s' not found.",
                    host);
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
            errmsg = sqlite3_errmsg (_sqlite3_db);
            *error = guardian_error_new (
                    "%s",
                    errmsg);
            sqlite3_finalize(handle);
            return -1;
            break;
        case SQLITE_ROW:
            *error = guardian_error_new (
                    "Multiple entries of host '%s'.\n",
                    host);
            sqlite3_finalize(handle);
            return -1;
            break;
        case SQLITE_DONE:
            sqlite3_finalize(handle);
            break;
    }

    return host_id;
}

