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

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <libguardian/libguardian.h>

#ifndef PLUGINDIR
#define PLUGINDIR "../../plugins"
#endif

#ifndef PLUGINSUBDIR
#define PLUGINSUBDIR ".libs"
#endif

#define BUFFER_SIZE 1024

enum
{
    OPTION_VERSION = 0,
    OPTION_VERBOSE,
    OPTION_HELP,
    OPTION_API_CHECK
};

/************************
 * Command-line options *
 ************************/
static struct option long_options[] = {
    {"version", 0, 0, 'V'},     /* OPTION_VERSION */
    {"verbose", 0, 0, 'v'},     /* OPTION_VERBOSE */
    {"help", 0, 0, 'h'},        /* OPTION_HELP    */
    {"api-check", 0, 0, 0},
    {0, 0, 0, 0}
};

static void
show_version ()
{
    printf ("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
    printf ("Copyright (c) 2012-2015 Stephan Arts\n");
    printf ("There is NO WARRANTY, to the extent permitted by law.\n");
    return;
}

static void
show_usage ()
{
    printf ("Usage: %s [options]\n", PACKAGE_NAME);
    printf ("\n");
    printf ("Options:\n");
    printf ("   --version  -V     Show version information\n");
    printf ("   --help     -h     Show usage information (this output)\n");
    printf ("              -v     Show verbose output\n");
    printf ("              -vv    Show very-verbose output\n");
    return;
}

int
main (int argc, char **argv)
{
    int     option_index = 0;
    int     c = 0;
    int     verbosity = 0;
    int     ch = 0;

    GuardianPlugin *plugin;
    GuardianPluginDB *db_plugin;
    char *plugin_path = malloc (200);
    GuardianError *error = NULL;

    while (1)
    {
        c = getopt_long (argc, argv, "vVh",
                long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
        case 0:
            switch (option_index)
            {
            case OPTION_VERSION:
                show_version ();
                exit (0);
                break;
            case OPTION_VERBOSE:
                verbosity = verbosity + 1;
                break;
            case OPTION_HELP:
                show_usage ();
                exit (0);
                break;
            case OPTION_API_CHECK:
                ch = 1;
                break;
            }
            break;
        case 'V':
            show_version ();
            exit (0);
            break;
        case 'h':
            show_usage ();
            exit (0);
            break;
        case 'v':
            verbosity = verbosity + 1;
            break;
        default:
            fprintf (stderr, "Try '%s --help' for more information\n", PACKAGE_NAME);
            exit (1);
            break;
        }
    }

    if (argc-optind > 1)
    {
        fprintf (stderr, "No plugin-name provided\n");
        return 1;
    }

    sprintf (
            plugin_path,
            "%s/%s/%s/%s.so",
            PLUGINDIR,
            argv[optind],
            PLUGINSUBDIR,
            argv[optind]);

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

    if (plugin->type != GUARDIAN_PLUGIN_DB)
    {
        fprintf(stderr, "Plugin '%s' is not a DB Plugin\n", argv[optind]);
        exit(1);
    }

    db_plugin = plugin;

    fprintf(stderr, "[ OK ] Plugin %s Loaded\n", argv[optind]);

    if (ch == 1) {

        if (db_plugin->db.connect == NULL) {
            fprintf(stderr, "[FAIL] plugin->db.connect is NULL\n");
            exit(1);
        }
        fprintf(stderr, "[ OK ] plugin->db.connect is set\n");

        if (db_plugin->db.disconnect == NULL) {
            fprintf(stderr, "[FAIL] plugin->db.disconnect is NULL\n");
            exit(1);
        }
        fprintf(stderr, "[ OK ] plugin->db.disconnect is set\n");

        if (db_plugin->host.get == NULL) {
            fprintf(stderr, "[FAIL] plugin->host.get is NULL\n");
            exit(1);
        }
        fprintf(stderr, "[ OK ] plugin->host.get is set\n");

    }

    exit(0);
}
