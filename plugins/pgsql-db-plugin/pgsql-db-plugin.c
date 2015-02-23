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

static void
_pgsql_db_set_key (
        const char *key,
        const char *value);

static void
_pgsql_db_connect (
        void);

static void
_pgsql_db_disconnect (
        void);

GuardianPlugin *
guardian_plugin_init ()
{
    const char *errors = NULL;
    int     err_offset;
    GuardianPluginDB *plugin;

    guardian_log_info ("Initialise PostgreSQL plugin");

    plugin = guardian_new (sizeof (GuardianPluginDB), 1);

    plugin->schema_version = 1;

    /* Copy the name */
    strncpy(plugin->db_name, "pgsql", 20);

    /* DB */
    plugin->db.set_key    = _pgsql_db_set_key;
    plugin->db.connect    = _pgsql_db_connect;
    plugin->db.disconnect = _pgsql_db_disconnect;

    /* Roles */
    //plugin->roles.add = _pgsql_add_role;

    return (GuardianPlugin *)plugin;
}

static void
_pgsql_db_set_key (
        const char *key,
        const char *value)
{

}

static void
_pgsql_db_connect (
        void)
{
}

static void
_pgsql_db_disconnect (
        void)
{
}
