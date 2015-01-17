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

/*
 * This application is used by the various plugin test-scripts as a framework
 * for testing the various plugins.
 */

#include <config.h>

#include <stdlib.h>

#include <stdio.h>

#include <fcntl.h>

#include <sys/types.h>
#include <unistd.h>

#include <zmq.h>

#include <time.h>

#include <libguardian/libguardian.h>

#ifndef PLUGINDIR
#define PLUGINDIR "../../plugins"
#endif

#ifndef PLUGINSUBDIR
#define PLUGINSUBDIR ".libs/"
#endif

#define BUFFER_SIZE 1024

int
main (int argc, char **argv)
{
    GuardianPlugin *plugin;
    char *plugin_path = malloc (200);
    GuardianError *error = NULL;

    if (argc < 2)
    {
        fprintf (stderr, "No plugin-name provided\n");
        return 1;
    }

    /*
     * Allow 10 items, this should be configurable via the command-line.
     */
    guardian_items_init (10);

    sprintf (
            plugin_path,
            "%s/%s/%s%s.so",
            PLUGINDIR,
            argv[1],
            PLUGINSUBDIR,
            argv[1]);

    fprintf(stderr, "Loading plugin: %s\n", plugin_path);

    int fd = open(plugin_path, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Open Failed");
        exit(1);
    }

    plugin = guardian_plugin_load ( plugin_path, &error );
    if (plugin == NULL)
    {
        fprintf(stderr, "%s\n", guardian_error_get_msg (error));
        exit(1);
    }

    fprintf(stderr, "Plugin %s Loaded\n", argv[1]);
    if (plugin->update_items == NULL) {
        fprintf(stderr, "update_items not set\n");
        exit(1);
    }

    if (plugin->get_update_time == NULL) {
        fprintf(stderr, "get_update_time not set\n");
        exit(1);
    }

    if (plugin->get_update_time() != 0) {
        exit(1);
    }

    plugin->update_items();

    if (plugin->get_update_time() == 0) {
        exit(1);
    }

    exit(0);
}
