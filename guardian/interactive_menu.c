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

#include <time.h>

#include <curses.h>

#include <unistd.h>
#include <string.h>

#include <openssl/sha.h>

#include "interactive_menu.h"
#include "search_dialog.h"
#include "timeframe_dialog.h"

void
show_interactive_menu (void)
{
    int     search_win = 0;
    int     time_win = 0;
    int     i;
    int     y,
            x;
    time_t  t;
    WINDOW *win = initscr ();
    start_color ();
    cbreak ();
    noecho ();
    nonl ();
    intrflush (win, FALSE);
    curs_set (0);

    keypad (win, TRUE);
    while (1)
    {
        werase (win);
        getmaxyx (win, y, x);

        time (&t);

        /*
         * Check if there is enough horizontal space to draw the package
         * name, version and date+time.
         */
        if (x > strlen (PACKAGE_NAME) + strlen (PACKAGE_VERSION) + 28)
        {
            wmove (win, 0, 1);
            waddstr (win, PACKAGE_NAME);

            wmove (win, 0, (x - 24) / 2);
            attrset (A_BOLD);
            waddnstr (win, ctime (&t), 24);
            attrset (A_NORMAL);

            wmove (win, 0, x - strlen (PACKAGE_VERSION) - 1);
            waddstr (win, PACKAGE_VERSION);
        } else
        {

            /*
             * Check if there is enough horizontal space to draw the package
             * name and date+time.
             */
            if (x > strlen (PACKAGE_NAME) + 26)
            {
                wmove (win, 0, 1);
                waddstr (win, PACKAGE_NAME);

                wmove (win, 0, x - 25);
                waddnstr (win, ctime (&t), 24);
            } else
            {

                /*
                 * Check if there is enough horizontal space to draw the
                 * date+time.
                 */
                if (x >= 24)
                {
                    wmove (win, 0, (x - 24) / 2);
                    waddnstr (win, ctime (&t), 24);
                }
            }
        }
        wmove (win, y - 1, 0);
        //waddnstr (win, "3.3k records", 12);
        waddnstr (win, "<CONN ERROR>", 12);

        wmove (win, y - 1, x - 5);
        waddnstr (win, "user", 4);

        /* Draw a horizontal line */
        wmove (win, 1, 0);
        hline ('-', x);

        wmove (win, 1, x - 4);
        waddstr (win, "[R]");

        wmove (win, 1, x - 8);
        waddstr (win, "[A]");

        wmove (win, 1, x - 30);
        waddstr (win, "[Time               ]");

        wmove (win, 1, x - 60);
        waddstr (win, "[Value                  ]");

        wmove (win, 1, 0);
        waddstr (win, "[Item             ]");

        if (time_win)
        {
            show_timeframe_dialog (win, 1, 5);
        }
        if (search_win)
        {
            show_search_dialog (win, 1, 5);
        }
        if (time_win == search_win)
        {
            /* Draw slider */
            wmove (win, 3, x - 1);
            vline (' ' | A_REVERSE, y - 4);

            wmove (win, 2, x - 1);
            addch (ACS_UARROW | A_REVERSE);

            wmove (win, 3, x - 1);
            addch (' ');

            wmove (win, y - 2, x - 1);
            addch (ACS_DARROW | A_REVERSE);
            /*******/

            wmove (win, 2, 1);
            waddstr (win,
                    "cpu.load.avg[1]   | 0.12");
            wmove (win, 2, x - 31);
            waddstr (win,
                    "| 2015/01/04 12:23:34 | x |");

            wmove (win, 3, 1);
            waddstr (win,
                    "cpu.load.avg[5]   | 0.12");
            wmove (win, 3, x - 31);
            waddstr (win,
                    "| 2015/01/04 12:23:34 | x |");
        }
        //waddch (win, i);

        //waddch (win, i);
        wmove (win, y, 0);

        if (search_win == 1)
        {
            if (search_dialog_input () != 0)
            {
                search_win = 0;
            }
        } else
        {
            if (time_win == 1)
            {
                if (timeframe_dialog_input () != 0)
                {
                    time_win = 0;
                }
            } else
            {
                i = wgetch (win);
                if (i == 'q')
                {
                    wclear (win);
                    wrefresh (win);
                    break;
                }
                if (i == 't')
                {
                    time_win = 1;
                    wclear (win);
                    wrefresh (win);
                }
                if (i == '/')
                {
                    search_win = 1;
                    wclear (win);
                    wrefresh (win);
                }
            }
        }
    }
    endwin ();
}
