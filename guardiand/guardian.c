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

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <signal.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/un.h>

#include <dirent.h>

#include <pthread.h>

#include <unistd.h>
#include <string.h>

#include <openssl/sha.h>

#include <libguardian/libguardian.h>

#include "assert.h"
#include "util.h"
#include "settings.h"
#include "scheduler.h"

enum {
    OPTION_VERSION = 0,
    OPTION_VERBOSE,
    OPTION_HELP,
    OPTION_FATAL_WARNINGS
};

/************************
 * Command-line options *
 ************************/
static struct option long_options[] = {
    {"version", 0, 0, 'V'}, /* OPTION_VERSION */
    {"help",    0, 0, 'h'}, /* OPTION_HELP    */
    {"fatal-warnings", 0, 0, 0}, /* OPTION_FATAL_WARNINGS */
    {0, 0, 0, 0}
};

static void
show_version ()
{
    printf ("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
    printf ("Copyright (c) 2012 Stephan Arts\n");
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
    printf ("\n");
    printf ("   --fatal-warnings  Make all warnings fatal\n");
    return;
}

void
process_signal (int s)
{
    switch (s)
    {
        case SIGINT:
            /**
             * Let's ignore further instances of SIGINT.
             */
            signal (SIGINT, SIG_IGN);
            guardian_scheduler_main_quit ();
            break;
        case SIGTERM:
            guardian_scheduler_main_quit ();
            break;
        default:
            break;
    }
    return;
}

/**
 * main
 * @argc: Number of elements in argv
 * @argv: Array of command-line options provided to the application
 *
 * @returns: Exit-code
 */
int
main (int argc, char **argv)
{
    int option_index = 0;
    int c = 0;
    int verbosity = 0;
    int log_level = 0;
    GuardianError *error = NULL;

    DIR *plugin_dir = NULL;
    struct dirent *dirp;
    int i = 0;

    char plugin_path[1024];

    struct sigaction sa;

    sa.sa_handler = process_signal;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);

    sigaction (SIGINT, &sa, NULL);
    sigaction (SIGTERM, &sa, NULL);

    GuardianSettings *settings = NULL;

    GuardianPlugin *plugin;
    GuardianSource *source;

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
                        show_version();
                        exit(0);
                        break;
                    case OPTION_HELP:
                        show_usage();
                        exit(0);
                        break;
                    case OPTION_FATAL_WARNINGS:
                        guardian_set_fatal_asserts ( TRUE );
                        break;
                }
                break;
            case 'V':
                show_version ();
                exit(0);
                break;
            case 'h':
                show_usage();
                exit(0);
                break;
            case 'v':
                verbosity = verbosity + 1;
                break;
            default:
                fprintf(stderr, "Try '%s --help' for more information\n", PACKAGE_NAME);
                exit(1);
                break;
        }
    }

    if (verbosity == 0)
    {
        guardian_log_init (0);

        switch (log_level)
        {
            default:
                guardian_log_mask (log_level);
        }
    }
    else
    {
        guardian_log_init (1);

        switch (verbosity)
        {
            case 1:
                guardian_log_mask (GUARDIAN_LOG_INFO);
                break;
            case 2:
                printf("DEBUG\n");
                guardian_log_mask (0);
                break;
        }
    }

    /**
     * Initialise guardian library
     */
    libguardian_init ();


    /**
     * Load settings from file
     */

    settings = guardian_settings_load (SYSCONFDIR"/guardian.conf", NULL);

    guardian_settings_get (settings, "key");
    
    guardian_set_timezone_gmt_offset ( 120 );

    /**
     * Load all plugins from PLUGINDIR
     */
    plugin_dir = opendir (PLUGINDIR);   
    if (plugin_dir != NULL)
    {
        while ((dirp = readdir (plugin_dir)) != NULL)
        {
            i = snprintf (plugin_path, 1024, "%s/%s", PLUGINDIR, dirp->d_name);
            if (i < 0)
            {
                /* An error occurred */
            }
            else
            {
                if (i > 1023)
                {
                    /* Error, prevented buffer overflow... path too long */
                    guardian_log_warning ( "%s", "Can not load plugin, plugin-path exceeds 1024 bytes");
                }
                else
                {
                    /**
                     * Only load .so files, the rest can't be a plugin.
                     */
                    if (strcmp (&plugin_path[i-3], ".so") == 0)
                    {
                        plugin = guardian_plugin_load ( plugin_path, &error );
                        if ( plugin == NULL )
                        {
                            guardian_log_warning ( "%s", guardian_error_get_msg (error));
                            guardian_error_free (error);
                            error = NULL;
                        }
                        else
                        {
                            printf("Load plugin: %s\n", plugin_path);
                            guardian_plugin_register_types ( plugin );
                        }
                    }
                }
            }
        }
    }

    source = guardian_source_new ("syslog", "/var/log/auth.log", &error);
    guardian_scheduler_add_source ( source );

    source = guardian_source_new ("syslog", "/var/log/boot.log", &error);
    guardian_scheduler_add_source ( source );

    source = guardian_source_new ("syslog", "/var/log/syslog", &error);
    guardian_scheduler_add_source ( source );

    /** Start the main loop */
    guardian_scheduler_main ( );

    exit (0);
}
