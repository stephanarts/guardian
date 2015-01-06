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

void
show_timeframe_dialog (WINDOW *parent, int x, int y)
{
    WINDOW *win = subwin (parent, 5, 20, y, x);
    wborder (win, 0, 0, 0, 0, 0, 0, 0, 0);
    wmove (win, 0, 2);
    waddstr (win, "[ Timeframe ]");

    wmove (win, 1, 3);
    waddstr (win, "1 hour");
    wmove (win, 2, 1);
    waddstr (win, "> 2 hours");
    wmove (win, 3, 3);
    waddstr (win, "8 hours");

    wmove (win, 4, 18);
    waddch (win, 'v');
}

int
timeframe_dialog_input (void)
{
    int     i = getch ();
    switch (i)
    {
        /* Close window on ENTER */
    case KEY_ENTER:
    case '\r':
    case '\n':
        return 1;
        break;
        /* Reserve up and down for autocomplete (or something) */
    case KEY_UP:
    case KEY_DOWN:
        break;
    case KEY_RIGHT:
    case KEY_LEFT:
        break;
    }
    return 0;
}
