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

#define SEARCH_BUFFER_SIZE 50
static int search_window_size = 38;
static char search_buffer[SEARCH_BUFFER_SIZE];
static int search_buffer_len = 0;
static int search_cursor = 0;

void
show_search_dialog (WINDOW *parent, int x, int y)
{
    int     i;
    WINDOW *win = subwin (parent, 3, 40, y, x);
    wborder (win, 0, 0, 0, 0, 0, 0, 0, 0);
    wmove (win, 0, 2);
    waddstr (win, "[ Search query ]");
    wmove (win, 1, 1);
    whline (win, ' ', x - 12);
    //waddnstr (win, search_buffer, SEARCH_BUFFER_SIZE);

    for (i = 0; i < search_buffer_len && i < search_window_size; ++i)
    {

        wmove (win, 1, 1 + i);

        if (i == search_cursor)
        {
            waddch (win, search_buffer[i] | A_REVERSE);
        } else
        {
            waddch (win, search_buffer[i]);
        }
    }

    if (search_cursor == search_buffer_len &&
            search_cursor < search_window_size)
    {
        wmove (win, 1, 1 + search_cursor);
        waddch (win, ' ' | A_REVERSE);
    }
}

int
search_dialog_input (void)
{
    int     a = search_cursor;
    char    b;

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
        search_cursor++;
        if (search_cursor > search_buffer_len)
        {
            search_cursor = search_buffer_len;
        }
        break;
    case KEY_LEFT:
        search_cursor--;
        if (search_cursor < 0)
        {
            search_cursor = 0;
        }
        break;
    case KEY_DC:               /* DELETE CHARACTER */
        if (search_cursor != search_buffer_len)
        {
            a = search_cursor;
            while (a < search_buffer_len)
            {
                search_buffer[a] = search_buffer[a + 1];
                a++;
            }
            search_buffer[a] = ' ';

            search_buffer_len--;
        }
        break;
    case KEY_BACKSPACE:
        if (search_cursor > 0)
        {
            a = search_cursor;
            while (a < search_buffer_len)
            {
                search_buffer[a - 1] = search_buffer[a];
                a++;
            }
            search_buffer[a - 1] = ' ';

            search_cursor--;
            search_buffer_len--;
        }
        break;
    default:
        if ((i >= 'a' && i <= 'z') ||   /* lower-case characters */
                (i >= 'A' && i <= 'Z') ||       /* upper-case characters */
                (i >= '0' && i <= '9') ||       /* numbers 0-9 */
                (i == '(' || i == ')') ||       /* '(' ')' */
                (i == ' ' || i == '|') ||       /* ' ' and '|' */
                (i == ',' || i == '.') ||       /* '.' and ',' */
                (i == '\\' || i == '/') ||      /* '\\' and '/' */
                (i == '=' || i == '%')
                )
        {

            a = search_cursor;
            while (a < search_buffer_len)
            {
                b = search_buffer[a];
                search_buffer[a] = i;
                i = b;
                a++;
            }
            search_buffer[a] = (char)i;
            search_cursor++;
            search_buffer_len++;
        }
        break;
    }
    return 0;
}
