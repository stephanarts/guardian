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

#include <string.h>

#include <pcre.h>

#include "memory.h"
#include "error.h"
#include "source.h"
#include "entry.h"
#include "sourceengine.h"
#include "sourcetype.h"
#include "field.h"

static size_t          n_fields;
static GuardianField **fields;

struct _GuardianFieldEntry
{
    GuardianEntry *entry;
    int            len;
    char          *data;
};

struct _GuardianField
{
    char *name;
    GuardianSourcetype *type;
    GUFieldSortFunc sort_func;
}; 

void
guardian_field_register (
        char *name,
        GuardianSourcetype *type,
        GUFieldSortFunc func )
{
    int i;
    GuardianField **_fields = (GuardianField **)(malloc (sizeof(GuardianField *)*n_fields+1));
    GuardianField *field = (GuardianField *)malloc (sizeof(GuardianField));
    field->name = name;
    field->type = type;
    field->sort_func = func;

    for (i = 0; i < n_fields; ++i)
    {
        _fields[i] = fields[i];
    }

    _fields[n_fields] = field;

    free (fields);
    fields = _fields;
}
