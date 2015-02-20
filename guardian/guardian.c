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

#define COLOR_GREY 8

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

#include <time.h>

#include <curses.h>

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

#include <zmq.h>

#include <openssl/sha.h>

#include "interactive_menu.h"
#include "shell.h"

#define SEARCH_BUFFER_SIZE 50
char    search_buffer[SEARCH_BUFFER_SIZE];
int     search_cursor = 0;

enum
{
    OPTION_VERSION = 0,
    OPTION_VERBOSE,
    OPTION_HELP,
    OPTION_FATAL_WARNINGS,
    OPTION_HOST,
    OPTION_PORT,
    OPTION_SILENT
};

/************************
 * Command-line options *
 ************************/
static struct option long_options[] = {
    {"version", 0, 0, 'V'},     /* OPTION_VERSION */
    {"help", 0, 0, 'h'},        /* OPTION_HELP    */
    {"fatal-warnings", 0, 0, 0},/* OPTION_FATAL_WARNINGS */
    {"host", 0, 0, 'H'},        /* OPTION_HOST    */
    {"port", 0, 0, 'P'},        /* OPTION_PORT    */
    {"silent", 0, 0, 's'},      /* OPTION_SILENT  */
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
    printf ("              -i     Run interactive menu\n");
    printf ("   --host     -H     Connect to guardian host\n");
    printf ("                     (default: 127.0.0.1)\n");
    printf ("   --port     -P     Connect on port\n");
    printf ("                     (default: 5678)\n");
    printf ("   --silent   -s     Silent CLI\n");
    printf ("                     (do not print shell prompt)\n");
    printf ("\n");
    printf ("   --fatal-warnings  Make all warnings fatal\n");
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
    int     option_index = 0;
    int     c = 0;
    int     verbosity = 0;
    int     silent = 0;

    while (1)
    {
        c = getopt_long (argc, argv, "vVhiHPs",
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
            case OPTION_HELP:
                show_usage ();
                exit (0);
                break;
            case OPTION_FATAL_WARNINGS:
                break;
            case OPTION_HOST:
                break;
            case OPTION_PORT:
                break;
            case OPTION_SILENT:
                silent = 1;
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
        case 'i':
            show_interactive_menu ();
            exit (0);
            break;
        case 'H':
        case 'P':
            break;
        case 's':
            silent = 1;
            break;
        default:
            fprintf (stderr, "Try '%s --help' for more information\n", argv[0]);
            exit (1);
            break;
        }
    }

    int     no_linger = 0;
    void   *ctx = zmq_ctx_new ();
    void   *server = zmq_socket (ctx, ZMQ_REQ);

    zmq_connect (server, "tcp://0.0.0.0:5678");

    zmq_setsockopt (server, ZMQ_LINGER, &no_linger, sizeof (no_linger));

    show_shell (silent);

    zmq_close (server);

    exit (0);
}
