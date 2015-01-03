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

/*
 * This application is used by the various plugin test-scripts
 * as a framework for testing.
 */

#include <config.h>

#include <stdlib.h>

#include <stdio.h>

#include <string.h>

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <libguardian/libguardian.h>

#define BUFFER_SIZE 1024

enum {
    OPTION_VERSION = 0,
    OPTION_HELP,
    OPTION_COUNT
};

/************************
 * Command-line options *
 ************************/
static struct option long_options[] = {
    {"version",     0, 0, 'V'}, /* OPTION_VERSION */
    {"help",        0, 0, 'h'}, /* OPTION_HELP */
    {0, 0, 0, 0}
};

static void
show_version ()
{
    printf ("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
    printf ("Copyright (c) 2014 Stephan Arts\n");
    printf ("There is NO WARRANTY, to the extent permitted by law.\n");
    return;
}

static void
show_usage ()
{
    printf ("Usage: %s [options]\n",
            PACKAGE_NAME);
    printf ("\n");
    printf ("Options:\n");
    printf ("   --version  -V     Show version information\n");
    printf ("   --help     -h     Show usage information (this output)\n");
    printf ("\n");
    printf ("   --test-verify <string>\n");
    return;
}

int
main (int argc, char **argv)
{
    int option_index = 0;
    int c = 0;
    int ret = 0;

    struct stat buffer;
    int fd;

    int _test_verify = 0;

    char data_buffer[BUFFER_SIZE];

    while (1)
    {
        c = getopt_long (argc, argv, "Vh",
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
            default:
                fprintf(stderr, "Try '%s --help' for more information\n", argv[0]);
                exit (1);
                break;
        }
    }

    argc-=optind;
    argv+=optind;

    libguardian_init();

    ret = guardian_register_expression (
        "MESSAGE",
        "^(.+)$");

    ret = guardian_register_expression (
        "URL",
        "%{TIMESTAMP} %{IP}");

    ret = guardian_register_expression (
        "URL",
        "%{TIMESTAMP}%{IP}");

    ret = guardian_register_expression (
        "URL",
        "%{TIMESTAMP}");

    printf("%d", ret);

    exit(ret);
}
