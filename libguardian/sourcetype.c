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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#include <string.h>

#include <pcre.h>

#include "memory.h"
#include "error.h"
#include "source.h"
#include "sourceengine.h"
#include "sourcetype.h"

#define MAX_PATTERN_COUNT 100
#define OFFSET_SIZE 99

struct _GuardianSourcetype
{
    /**
     * The SourceType name, a unique identifier. (eg, syslog)
     */
    char  *name;

    /**
     * The expression used to identify a single entry from the source.
     * (eg. ^(.*)$ for a single line entry, 
     * ^(.*\n.*)$ for a double-line entry.
     */
    char  *expression;

    /**
     * PCRE context used to parse retrieve the entries.
     */
    pcre  *pcre_context;

    /**
     * SourceEngine used for managing the sources of this type.
     */
    GuardianSourceEngine *engine;
};

int    _n_guardian_source_types = 0;
static GuardianSourcetype **_guardian_source_types = NULL;

GuardianSourcetype *
guardian_sourcetype_register (
        const char           *name,
        const char           *expression,
        GuardianSourceEngine *engine )
{
    GuardianSourcetype *type;
    const char *errors = NULL;
    int err_offset;
    int i;
    GuardianSourcetype **_source_types;

    /* Check if the name is not already taken */

    /* Allocate Sourcetype */    
    type = guardian_new (sizeof (GuardianSourcetype), 1); 

    /* Set the variables */
    type->name = guardian_new (sizeof (char), strlen (name)+1);
    strcpy (type->name, name);

    type->expression = guardian_new (sizeof (char), strlen (expression)+1);
    strcpy (type->expression, expression);

    type->pcre_context = pcre_compile (
            expression,
            PCRE_FIRSTLINE |
            PCRE_MULTILINE |
            PCRE_NEWLINE_ANYCRLF,
            &errors,
            &err_offset,
            NULL);

    type->engine = engine;

    if ( _guardian_source_types == NULL )
    {
        _guardian_source_types = guardian_new (sizeof (GuardianSourcetype *), 1);
        _guardian_source_types[0] = type;
        _n_guardian_source_types = 1;
    }
    else
    {
        _source_types = guardian_new (sizeof (GuardianSourcetype *), _n_guardian_source_types+1);

        for (i = 0; i < _n_guardian_source_types; ++i)
        {
            if (strcmp (_guardian_source_types[i]->name, type->name) < 0 )
            {
                _source_types[i] = _guardian_source_types[i];
            }
            else
            {
                break;
            }
        }
        _source_types[i] = type;

        for (; i < _n_guardian_source_types+1; ++i)
        {
            _source_types[i+1] = _guardian_source_types[i];
        }

        free (_guardian_source_types);
        _guardian_source_types = _source_types;
        _n_guardian_source_types++;
    }

    return type;
}

GuardianSourcetype *
guardian_sourcetype_lookup (
        const char *name )
{
    int i_min = 0;
    int i_max = _n_guardian_source_types-1;
    int i_mid;
    int r;


    while (i_max >= i_min)
    {
        i_mid = (i_min+i_max) / 2;

        r = strcmp (_guardian_source_types[i_mid]->name, name);
        if (r > 0)
        {
            i_max = i_mid - 1;
        }
        if (r < 0)
        {
            i_min = i_mid + 1;
        }
        if (r == 0)
        {
            return _guardian_source_types[i_mid];
        }
    }

    return NULL;
}

int
guardian_sourcetype_get_entry (
        GuardianSourcetype *source_type,
        const char *buffer,
        size_t buffer_len,
        size_t start_offset,
        const char **entry,
        size_t *entry_offset,
        size_t *entry_len,
        GuardianError **error )
{
    int offsets[OFFSET_SIZE];
    int ret;
    char *err_str;

    ret = pcre_exec (
            source_type->pcre_context,
            NULL,
            buffer,
            buffer_len,
            start_offset,
            PCRE_NOTEMPTY,
            offsets,
            OFFSET_SIZE);

    if (ret >= 0)
    {
        pcre_get_substring (
                buffer,
                offsets,
                ret,
                1,
                entry);
        *entry_offset = offsets[0];
        *entry_len = offsets[1];
        return 0;
    }

    if ( error != NULL )
    {
        switch (ret)
        {
            default:
                err_str = "UNKNOWN PCRE Parse error";
        }

        *error = guardian_error_new (err_str);
    }

    return 1;
}

GuardianSourceEngine *
guardian_sourcetype_get_engine (
        GuardianSourcetype *source_type)
{
    return source_type->engine;
}
