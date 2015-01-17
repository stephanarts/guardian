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

#ifdef HAVE_SYS_SYSLOG_H
#include <sys/syslog.h>
#endif

#include <stdlib.h>
#include <string.h>

#include <stdarg.h>

#include <arpa/inet.h>

#include <time.h>


#include "assert.h"
#include "error.h"
#include "log.h"
#include "types.h"
#include "itemtype.h"
#include "item.h"
#include "memory.h"

struct _GuardianItem
{
    char   *name;
    GuardianItemType type;

    double  interval;
    int     active;
    int     remote;

    time_t  last_update;
};

static GuardianItem *_items = NULL;
static size_t _n_max_items = 0;
static size_t _n_items = 0;


void
guardian_items_init (
        size_t n_max_items)
{
    if (_items != NULL)
    {
        guardian_assert_critical (
                "items already initialized!");
        return;
    }
    _items = guardian_new (
            sizeof (GuardianItem),
            n_max_items);

    _n_max_items = n_max_items;
}

GuardianItem *
guardian_item_register (
        const char *name,
        GuardianItemType type,
        double interval,
        int active,
        int remote,
        GuardianError **error)
{
    GuardianItem *item = NULL;

    /**
     * Check if the item-registry has space for a new
     * entry.
     */
    if (_n_max_items == _n_items)
    {
        guardian_log_error (
                "Max items reached (%d)",
                _n_max_items);
        return NULL;
    }
    item = &_items[_n_items];

    _n_items++;

    guardian_log_debug ("New Item: %s", name);

    item->last_update = -1;

    return item;
}

void
guardian_item_unregister (
        GuardianItem *item)
{

}

void
guardian_item_get_interval (
        GuardianItem *item,
        double *interval)
{
    (*interval) = item->interval;
}

void
guardian_item_set_interval (
        GuardianItem *item,
        double interval)
{
    item->interval = interval;
}

void
guardian_item_get_last_update (
        GuardianItem *item,
        time_t *last_update)
{
    (*last_update) = item->last_update;
}

void
guardian_item_set_last_update (
        GuardianItem *item,
        time_t last_update)
{
    item->last_update = last_update;
}
