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
#include "shell/connect.h"
#include "shell/disconnect.h"
#include "shell/help.h"
#include "shell/exit.h"
#include "shell/register.h"
#include "client.h"

#define MAX_TOKENS 32

struct Command {
    char *name;
    int (*parse)(char **tokens, int n_tokens);
};

static struct Command commands[] = {
    {"connect",     parse_connect},
    {"disconnect",  parse_disconnect},
    {"help",        parse_help},
    {"exit",        parse_exit},
    {"register",    parse_register},
    {NULL,NULL}
};

static void
parse (char **tokens, int n_tokens) {

    if (n_tokens == 0) {
        return;
    }

    struct Command *command = commands;
    while (command->name != NULL) {
        if (strcmp(command->name, tokens[0]) == 0) {
            command->parse(tokens, n_tokens);
            break;
        }
        command++;
    }

    if (command->name == NULL) {
        fprintf(stdout, "Unknown token: %s\n", tokens[0]);
        return;
    }

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
