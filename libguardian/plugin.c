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

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#include <errno.h>

#include "error.h"
#include "plugin.h"

void
guardian_plugin_register_types ( GuardianPlugin *plugin )
{
    if ( plugin->register_types != NULL )
    {
        plugin->register_types ( plugin );
    }
}

void
guardian_plugin_extract_fields (
        GuardianPlugin *plugin,
        const char *entry )
{
    if ( plugin->extract_fields != NULL )
    {
        plugin->extract_fields ( plugin, entry );
    }
}

GuardianPlugin *
guardian_plugin_load ( char *path, GuardianError **error )
{
    void *handle = NULL;
    GuardianPlugin *plugin;
    int error_sv;
    char error_msg[200];
    char *ld_error = NULL;

    GuardianPlugin *(*_plugin_init)(void);

    handle = dlopen (path, RTLD_NOW );
    error_sv = errno;

    if ( handle )
    {
        /**
         * Clear error string (if any)
         */
        dlerror();

        /**
         * Retrieve the init function, do not call it yet
         */
        _plugin_init = ((GuardianPlugin *(*)())dlsym (handle, "guardian_plugin_init"));

        /**
         * Find out if anything goes wrong
         */
        ld_error = dlerror();
        if (ld_error == NULL)
        {
            /**
             * Call the initialisor, and create the plugin-object
             */
            plugin = _plugin_init();
            if ( plugin != NULL )
            {
                plugin->handle = handle;
                handle = NULL;
                return plugin;
            }
        }
        
    }
    if ( error )
    {
        /**
         * If something went wrong during the symbol-lookup proces.
         */
        if (ld_error != NULL)
        {
            *error = guardian_error_new ("Can not load plugin: '%s'", ld_error);
        }
        else
        {
            /**
             * If something went wrong during the dlopen() process
             */
            if ( strerror_r (error_sv, error_msg, 200) == 0)
            {
                *error = guardian_error_new ("Can not load plugin: '%s: %s'", path, error_msg);
            }
            else
            {
                /**
                 * If something else went wrong...
                 */
                *error = guardian_error_new ("Can not load plugin: '%s'", path);
            }
        }
    }

    return NULL;
}
