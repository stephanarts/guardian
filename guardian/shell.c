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

#include <termios.h>
#include <ctype.h>

#include <openssl/sha.h>

#include "shell.h"
#include "client.h"

#define MAX_TOKENS 32

static void
parse (char **tokens, int n_tokens) {
    char buf[256];
    struct termios saved_attributes, hide_pasword;

    if (n_tokens == 0) {
        return;
    }

    /* CONNECT */
    if (strcmp(tokens[0], "connect") == 0) {
        switch (n_tokens) {
            case 3:
                client_connect_pass(
                        "tcp://localhost:1234",
                        tokens[1],
                        tokens[2]);
                break;
            case 2:
                tcgetattr (STDIN_FILENO, &saved_attributes);
                hide_pasword = saved_attributes;
                hide_pasword.c_lflag &= ~ECHO;

                tcsetattr (STDIN_FILENO, TCSANOW, &hide_pasword);

                fprintf (stdout, "Password: ");
                if(fgets (buf, 256, stdin) != NULL)
                {
                    fprintf (stdout, "\n");
                    int l = strlen(buf);
                    if (buf[l-1] == '\n') {
                        buf[l-1] = '\0';
                    }

                    tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);

                    client_connect_pass(
                            "tcp://localhost:1234",
                            tokens[1],
                            buf);
                } else {
                    fprintf (stdout, "\n");
                    tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
                }
                break;
            default:
                printf("Connect Error\n");
        }
        return;
    }

    /* DISCONNECT */
    if (strcmp(tokens[0], "disconnect") == 0) {
         
        return;
    }

    if (strcmp(tokens[0], "help") == 0) {
        if (n_tokens == 1) {
            fprintf(stdout, "connect    - Connect to server\n");
            fprintf(stdout, "help       - Show this message\n");
        }
        if (n_tokens == 2) {
            if (strcmp(tokens[1], "connect") == 0) {
                fprintf(stdout, "connect <user> [password]\n");
            }

        }
        return;
    }

    /* ERROR */
    fprintf(stdout, "Parse error\n");
    return;
}

void
show_shell (int silent)
{
    char buf[256];

    if (silent == 0)
    {
        fprintf (stderr, "+------------------------------------+\n");
        fprintf (stderr, "| Guardian Interactive Shell         |\n");
        fprintf (stderr, "| Copyright 2012-2015 - Stephan Arts |\n");
        fprintf (stderr, "| Licensed under the BSD License     |\n");
        fprintf (stderr, "+------------------------------------+\n");
    }
    while (1)
    {
        if (silent == 0) {
            fprintf (stdout, "G> ");
        }

        if(fgets (buf, 256, stdin) != NULL)
        {
            /* Remove trailing newline */
            int l = strlen(buf);
            if (buf[l-1] == '\n') {
                buf[l-1] = '\0';
            }

            int i = 0;
            char *tokens[MAX_TOKENS];

            /* Split into tokens (separated by whitespace) */
            char *key = buf;
            char *token = strsep(&key, " \t");
            while(token != NULL) {
                if (*token != '\0') {
                    tokens[i] = token;
                    ++i;
                }
                token = strsep(&key, " \t");
            }

            parse (tokens, i);

            continue;
        }
        if (feof(stdin))
        {
            break;
        }
    }

}
