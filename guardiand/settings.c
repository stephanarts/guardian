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

#ifdef have_stdlib_h
#include <stdlib.h>
#endif

#include <stdio.h>

#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>

#include <time.h>

#include <libguardian/libguardian.h>

#include "settings.h"

static FILE *
open_config_file (const char *path) {

    FILE *f_config = NULL;
    struct stat _c_stat;

    if (stat(path, &_c_stat)) {
        int i = errno;
        fprintf(stderr,
                "Could not stat '%s' - %s\n",
                path,
                strerror(i));
        return NULL; 
    }
    if (!S_ISREG(_c_stat.st_mode)) {
        fprintf(stderr,
                "'%s' is not a file\n",
                path);
        return NULL; 
    }

    f_config = fopen(
            path,
            "r");
    if (f_config == NULL) {
        fprintf(stderr,
                "'%s' could not be opened\n",
                path);
    }
    return f_config;
};

typedef struct {
    char key[64];
    char value[64];
} Prop;

struct _GuardianSettings
{
    char   *path;

    Prop    props[32];
    int     n_props;
};

GuardianSettings *
guardian_settings_load (
        const char *path,
        GuardianError **error)
{
    GuardianSettings *settings = NULL;

    FILE *f_config = open_config_file (path);
    if (f_config == NULL) {
        return NULL;
    }

    settings = guardian_new(1, sizeof(GuardianSettings));
    settings->n_props = 0;

    char line[128];
    int i = 0;
    int n_props = 0;

    while( fgets (line, sizeof(line), f_config) != NULL)
    {
        if (line[0] == '#')
        {
            continue;
        }

        char *key = strtok(line,"=");
        char *value = strtok(NULL,"=");

        if (key && value) {

            /* Sanitize KEY value */
            {
                /* Remove leading whitespace */
                while (*key != '\0')
                {
                    if (*key == ' ' || *key == '\t')
                    {
                        key++;
                    } else {
                        break;
                    }
                }

                /* Remove traling whitespace */
                char *ptr = key;
                while (*ptr != '\0' && *ptr != ' ')
                {
                    ptr++;
                }
                if (*ptr == ' ') {
                    *ptr = '\0';
                }
            }

            /* Remove leading whitespace */
            while (*value != '\0' && *value != '\n')
            {
                if (*value == ' ' || *value == '\t')
                {
                    value++;
                } else {
                    break;
                }
            }

            if (value[strlen(value)-1] == '\n')
            {
                value[strlen(value)-1] = '\0';
            }

            /* Check if the key already exists */
            for (i = 0; i < n_props; ++i) {
                /* Overwrite it's value with the new entry */
                if(strcmp(settings->props[i].key, key) == 0) {
                    strncpy(settings->props[i].value, value, 64);
                    break;
                }
            }

            /* If i == n_props, value is new */
            if (i == n_props) {
                strncpy(settings->props[n_props].value, value, 64);
                strncpy(settings->props[n_props].key, key, 64);
                n_props++;
            }
        }
    }

    settings->n_props = n_props;

    return settings;
}

const char *
guardian_settings_get (
        GuardianSettings * settings,
        const char *key)
{
    int i;

    for (i = 0; i < settings->n_props; ++i) {
        if (strcmp(settings->props[i].key, key) == 0) {
            return settings->props[i].value;
        }
    }

    return NULL;
}

void
guardian_settings_free (
        GuardianSettings * settings)
{

}
