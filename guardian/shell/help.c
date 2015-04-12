/*-
 * Copyright (c) 2015 Stephan Arts. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.  *
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

int
parse_help (char **tokens, int n_tokens) {

    /* HELP */
    if (strcmp(tokens[0], "help") == 0) {
        if (n_tokens == 1) {
            fprintf(stdout, "connect    - Connect to server\n");
            fprintf(stdout, "disconnect - Disconnect from server\n");
            fprintf(stdout, "\n");
            fprintf(stdout, "register   - Register\n");
            fprintf(stdout, "list       - List\n");
            fprintf(stdout, "publish    - Publish\n");
            fprintf(stdout, "\n");
            fprintf(stdout, "help       - Show this message\n");
        }
        if (n_tokens == 2) {
            if (strcmp(tokens[1], "connect") == 0) {
                fprintf(stdout, "Usage:\n");
                fprintf(stdout, "  connect <user> [password]\n");
            }
            if (strcmp(tokens[1], "disconnect") == 0) {
                fprintf(stdout, "Usage:\n");
                fprintf(stdout, "  disconnect\\n\n");
            }

            if (strcmp(tokens[1], "register") == 0) {
                fprintf(stdout, "register <type> - Register an item\n");
                fprintf(stdout, "    host        - Register a host\n");
                fprintf(stdout, "    ns          - Register a namespace\n");
            }

            if (strcmp(tokens[1], "list") == 0) {
                fprintf(stdout, "list <type>           - List items\n");
                fprintf(stdout, "    host              - List hosts\n");
                fprintf(stdout, "    ns <host>         - List namespaces\n");
                fprintf(stdout, "    items <ns> <host> - List items\n");
                return 0;
            }
        }
        if (n_tokens == 3) {
            if (strcmp(tokens[1], "register") == 0) {
                if (strcmp(tokens[2], "host") == 0) {
                    fprintf(stdout, "register host <options>\n");
                }
                if (strcmp(tokens[2], "ns") == 0) {
                    fprintf(stdout, "register ns <options>\n");
                }
            }
        }
        return 0;
    }

    return -1;
}
