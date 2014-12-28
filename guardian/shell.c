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

#include <histedit.h>

#include <openssl/sha.h>

#include "shell.h"

static int  silent_shell = 0;
static int  shell_open = 0;
static int  line_nr = 0;
static char prompt[100];

static char *
print_prompt(EditLine *el)
{
    if (silent_shell == 1) {
        sprintf(prompt, "");
    } else {
        if (shell_open == 1) {
            sprintf(prompt, " ");
        } else {
            sprintf(prompt, "%d> ", line_nr);
        }
    }
    return prompt;
}

void
show_shell (int silent)
{
    EditLine *el = NULL;
    Tokenizer *t = NULL;
    int n_tokens = 0;
    const char **tokens = NULL;

    silent_shell = silent;

    /*
     * Separators:
     *   <tab> 
     *   <newline>
     *   <space>
     */
    t = tok_init(NULL);

    el = el_init(
        "guardian",
        stdin,
        stdout,
        stderr);

    el_set(el,EL_PROMPT, print_prompt);

    if (silent == 0) {
        fprintf(stderr, "+------------------------------------+\n");
        fprintf(stderr, "| Guardian Interactive Shell         |\n");
        fprintf(stderr, "| Copyright 2012-2014 - Stephan Arts |\n");
        fprintf(stderr, "| Licensed under the BSD License     |\n");
        fprintf(stderr, "+------------------------------------+\n");
    }

    while(1) {

        el_gets(el, NULL);

        shell_open = 1;

        const LineInfo *li = el_line(el);
        int i = tok_line (
                t,
                li,
                &n_tokens,
                &tokens,
                NULL,
                NULL);

        if (i == 0) {

            /*
            printf("---\n");
            for (i = 0;  i < n_tokens; ++i) {
                printf(" > %s\n", tokens[i]);
            }
            printf("---\n");
            */

            tok_reset(t);
            line_nr++;
            shell_open = 0;

            /* EXIT */
            if (n_tokens == 1 && 0 == strcmp(tokens[0], "exit")) {
                break;
            }

            /* Search */
            if (!strcmp(tokens[0], "Q") || !strcmp(tokens[0], "query")) {
                printf("Search...\n");
            }

            /* Set */
            if (!strcmp(tokens[0], "set")) {
                printf("Set...\n");
                if (n_tokens == 1) {
                    printf("ERR: no key/value\n");
                    continue;
                }
                printf("A");
            }
        } else {
            if (i < 0) {
                tok_reset(t);
                line_nr++;
                shell_open = 0;
            }
        }

        continue;
    }
}
