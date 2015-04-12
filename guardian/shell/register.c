/*-
 * Copyright (c) 2015 Stephan Arts. All Rights Reserved.
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

#include "client.h"

static char cmd[256];

int
parse_register (char **tokens, int n_tokens) {

    /* REGISTER */
    if (strcmp(tokens[0], "register") == 0) {
        if (client_connected() == 0) {
            fprintf(stdout, "Not connected to host\n");
            return -1;
        }
        if (n_tokens == 1) {
            fprintf(stdout, "missing argument\n");
            return -1;
        }

        if (strcmp(tokens[1], "host") == 0) {
            if (n_tokens < 3) {
                fprintf(stdout, "missing argument\n");
                return -1;
            }
            if (n_tokens > 3) {
                fprintf(stdout, "too many arguments\n");
                return -1;
            }

            fprintf(stdout, "<register host %s>\n", tokens[2]);
            int i = snprintf(cmd, 256, "register host %s", tokens[2]);
            char *resp = NULL;
            size_t resp_len;

            client_send_cmd (cmd, i, &resp, &resp_len);
        }

        if (strcmp(tokens[1], "ns") == 0) {
            if (n_tokens < 4) {
                fprintf(stdout, "missing argument\n");
                return -1;
            }
            if (n_tokens > 5) {
                fprintf(stdout, "too many arguments\n");
                return -1;
            }

            if (strcmp(tokens[3], "on") != 0) {
                fprintf(stdout, "invalid keyword %s", tokens[3]);
                return -1;
            }

            for (int a = 0; a < strlen(tokens[2]); ++a) {
                if (tokens[2][a] >= 'a' && tokens[2][a] <= 'z') {
                    tokens[2][a]-=32;
                }
            }

            fprintf(stdout, "<register ns %s on %s>\n",
                    tokens[2],
                    tokens[4]);


            int i = snprintf(cmd,
                    256,
                    "register ns %s on %s", tokens[2], tokens[4]);
            char *resp = NULL;
            size_t resp_len;

            client_send_cmd (cmd, i, &resp, &resp_len);

        }
        return 0;
    }

    return -1;
}
