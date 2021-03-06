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

#include <sys/types.h>
#include <pthread.h>

#include "memory.h"
#include "error.h"
#include "entry.h"
#include "source.h"
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
    char                *name;
    comparison_fn_t      comp_func;

    GuardianFieldEntry **base;
    size_t               nmemb;

    pthread_mutex_t	 mutex;
}; 

static int
_guardian_field_compare_name ( const GuardianField *a, const GuardianField *b )
{
    return strcmp (a->name, b->name);
}

GuardianField *
guardian_field_register (
        char *name,
        comparison_fn_t comp_func )
{
    int i;
    GuardianField **_fields = (GuardianField **)(malloc (sizeof(GuardianField *)*n_fields+1));
    GuardianField *field = (GuardianField *)malloc (sizeof(GuardianField));
    field->name = name;
    field->comp_func = comp_func;

    /**
     * Initialise the mutex
     */
    pthread_mutex_init (&field->mutex, NULL);

    /** Copy the fields to the new array */
    for (i = 0; i < n_fields; ++i)
    {
        _fields[i] = fields[i];
    }

    /** Append the new field */
    _fields[n_fields] = field;

    free (fields);
    fields = _fields;
    n_fields++;

    /** Sort the entries */
    qsort (fields, n_fields, sizeof (GuardianField *), (comparison_fn_t) _guardian_field_compare_name);

    return field;
}

/**
 * guardian_field_lookup:
 * @name: The field name
 *
 * @Return: the corresponding GuardianField, or NULL.
 */
GuardianField *
guardian_field_lookup ( const char *name )
{
    int i_max = n_fields - 1;
    int i_min = 0;
    int i_mid;
    int r;
    GuardianField *ptr;

    while (i_max >= i_min )
    {
        i_mid = (i_min + i_max) / 2;

        r = strcmp (fields[i_mid]->name, name);

        if (r > 0)
        {
            i_max = i_mid - 1;
            continue;
        }
        if (r < 0)
        {
            i_min = i_mid + 1;
            continue;
        }
        if (r == 0)
        {
            return fields[i_mid];
        }
    }

    return NULL;
}

/**
 * guardian_field_add_entry:
 * @field: The field object
 * @entry: The entry to add
 * @len:   The length of the data field
 * @data:  The data associated to this entry in this field
 *
 * NOTE:
 * It's not a good idea to reallocate this array a few milion times.
 * It is faster to allocate multitudes of 1024 pointers and keep the
 * exact length of the array stored.
 */
void
guardian_field_add_entry (
        GuardianField *field,
        GuardianEntry *entry,
        size_t         len,
        char          *data )
{
    GuardianFieldEntry **entries;
    GuardianFieldEntry *f_entry = (GuardianFieldEntry *)malloc (sizeof (GuardianFieldEntry));
    int i = 0;

    /** Initialise the field_entry object */
    f_entry->entry = entry;
    f_entry->len = len;
    f_entry->data = (char *)malloc(len*sizeof(char));
    strncpy (f_entry->data, data, len);

    /** Lock the field */
    pthread_mutex_lock (&field->mutex);

    entries = (GuardianFieldEntry **) malloc (sizeof (GuardianFieldEntry *) * (field->nmemb + 1));

    for (; i < field->nmemb && field->comp_func (f_entry, field->base[i]) < 0; ++i)
    {
        entries[i] = field->base[i];
    }
    entries[i] = f_entry;

    for (; i < field->nmemb; ++i)
    {
        entries[i+1] = field->base[i];
    }

    free (field->base);
    field->base = entries;
    field->nmemb++;

    /** Unlock the field */
    pthread_mutex_unlock (&field->mutex);
}
