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
parse_connect (char **tokens, int n_tokens) {

    char buf[256];
    char cmd[256];
    struct termios saved_attributes, hide_pasword;

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
        return 0;
    }

    return -1;
}
