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


#include "error.h"
#include "log.h"
#include "types.h"
#include "itemtype.h"
#include "item.h"

struct _GuardianItem
{
    char   *name;
    GuardianItemType type;

    double  interval;
    int     active;
    int     remote;

    time_t  last_update;
};

GuardianItem *
guardian_item_register (
        const char *name,
        GuardianItemType type,
        double interval,
        int active,
        int remote,
        GuardianError **error)
{
    GuardianItem *item = (GuardianItem *)malloc (sizeof (GuardianItem));

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
guardian_items_init ()
{
    //guardian_db_get_items ();
}

double
guardian_item_get_interval (GuardianItem *item)
{
    return item->interval;
}

void
guardian_item_set_interval (
        GuardianItem *item,
        double interval)
{
    item->interval = interval;
}

time_t
guardian_item_get_last_update (GuardianItem *item)
{
    return item->last_update;
}

void
guardian_item_set_last_update (
        GuardianItem *item,
        time_t last_update)
{
    item->last_update = last_update;
}