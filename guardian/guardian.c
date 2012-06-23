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

#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/un.h>

#include <unistd.h>
#include <string.h>

#include <openssl/sha.h>

#include <libguardian/libguardian.h>

#include "assert.h"
#include "util.h"

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

char *test_buffer = "Jan 10 11:11:11 aslk;djfa;slkjdf\nJan 11 12:12:12 ;lkjdfsa;lkjfdasdfa";

/********
 * main *
 ********/
int
main (int argc, char **argv)
{
    int option_index = 0;
    int c = 0;
    int verbosity = 0;
    int log_level = 0;
    GuardianError *error = NULL;

    char *plugin_path = "/usr/local/lib/guardian/syslog.so";

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

    plugin = guardian_plugin_load ( plugin_path, &error );
    if ( plugin == NULL )
    {
        guardian_log_warning ( "%s", guardian_error_get_msg (error));
        guardian_error_free (error);
        error = NULL;
    }
    else
    {
        guardian_plugin_register_types ( plugin );
    }

    source = guardian_source_new ("syslog", "/var/log/auth.log", &error);
    if ( source == NULL )
    {
        guardian_log_warning ( "%s", guardian_error_get_msg (error));
        guardian_error_free (error);
        error = NULL;
    }
    else
    {
        guardian_source_update ( source );
    }

    exit (0);
}
