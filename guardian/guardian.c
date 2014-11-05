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

#include <openssl/sha.h>

#define         SOCK_PATH "/tmp/guardian.sock"

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
    printf ("              -i     Run interactive menu\n");
    printf ("\n");
    printf ("   --fatal-warnings  Make all warnings fatal\n");
    return;
}

static void
run_menu (void)
{
    int i;
    int y, x;
    time_t t;
    WINDOW *win = initscr();
    while(1) {
        werase(win);
        getmaxyx(win, y, x);

        time(&t);

        /* Check if there is enough horizontal space
         * to draw the package name, version and date+time.
         */
        if (x > strlen(PACKAGE_NAME)+strlen(PACKAGE_VERSION)+28) {
            wmove(win, 0, 1);
            waddstr(win, PACKAGE_NAME);

            wmove(win, 0, (x-24)/2);
            waddnstr(win, ctime(&t), 24);

            wmove(win, 0, x-strlen(PACKAGE_VERSION)-1);
            waddstr(win, PACKAGE_VERSION);
        } else {
            /* Check if there is enough horizontal space
             * to draw the package name and date+time.
             */
            if (x > strlen(PACKAGE_NAME)+26) {
                wmove(win, 0, 1);
                waddstr(win, PACKAGE_NAME);

                wmove(win, 0, x-25);
                waddnstr(win, ctime(&t), 24);
            } else {
                /* Check if there is enough horizontal space
                 * to draw the date+time.
                 */
                if (x >= 24) {
                    wmove(win, 0, (x-24)/2);
                    waddnstr(win, ctime(&t), 24);
                }
            }
        }

        WINDOW *w = subwin(win, y-1, x, 1, 0);
        wborder(w, 0, 0, 0, 0, 0, 0, 0, 0);
        wmove(w, 1, 1);
        i = wgetch(win);
        if (i == 'q') {
            wclear(win);
            wrefresh(win);
            break;
        }
    }
    endwin();
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

    while (1)
    {
        c = getopt_long (argc, argv, "vVhi",
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
            case 'i':
                run_menu();
                exit(0);
                break;
            default:
                fprintf(stderr, "Try '%s --help' for more information\n", argv[0]);
                exit (1);
                break;
        }
    }

    exit(0);
}
