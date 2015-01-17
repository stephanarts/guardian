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

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_UTMPX_H
#include <utmpx.h>
#endif

#include <string.h>

#include <pcre.h>

#include <errno.h>

#include <time.h>

#include <sys/stat.h>

#include <stdarg.h>
#include <libguardian/libguardian.h>

#define PLUGIN_NAME "system-plugin"

enum
{
    ITEM_SYSTEM_UPTIME = 0,
    ITEM_COUNT
};


static GuardianItem *items[ITEM_COUNT];
static GuardianValue *update_cache[ITEM_COUNT];

static void register_system_items (void);
static void _update_items (void);
static time_t _get_update_time (void);

time_t  last_update_time;

GuardianPlugin *
guardian_plugin_init ()
{
    const char *errors = NULL;
    GuardianPlugin *plugin;
    GuardianItem *item = NULL;

    guardian_log_info ("Initialise system plugin");

    plugin = guardian_plugin_new ();
    plugin->update_items = _update_items;
    plugin->get_update_time = _get_update_time;

    register_system_items ();

    return plugin;
}

static void
guardian_plugin_item_update (
        GuardianItem *item)
{
}

static void
guardian_plugin_rescan_items (void)
{

}

static void
register_system_items (void)
{
    items[ITEM_SYSTEM_UPTIME] = guardian_item_register (
            "system.uptime",
            GUARDIAN_ITEMTYPE_INT,
            60,
            TRUE,
            FALSE,
            NULL);

    if (items[ITEM_SYSTEM_UPTIME] == NULL)
    {
        guardian_log_error (
                "Plugin '%s': Could not create Item '%s'.",
                PLUGIN_NAME,
                "system.uptime");
    }
}

static void
_update_items (void)
{
    time_t  new_time;
    time_t  item_time;
    time_t  uptime;
    double  t;
    double  i;

    time (&new_time);

    GuardianValue *value = guardian_value_new (items[ITEM_SYSTEM_UPTIME]);

    guardian_item_get_last_update (
            items[ITEM_SYSTEM_UPTIME],
            &item_time);
    guardian_item_get_interval (
            items[ITEM_SYSTEM_UPTIME],
            &i);

    t = difftime (item_time, new_time);

    if (t >= i)
    {

#if 0

#ifdef __FREEBSD__
        clock_gettime (CLOCK_UPTIME, &uptime);
#endif                          /* __FREEBSD__ */

#endif

#ifdef HAVE_UTMPX_H
        struct utmpx *ent;
        while ((ent = getutxent ()))
        {
            if (ent->ut_type == BOOT_TIME)
            {
                uptime = difftime (ent->ut_tv.tv_sec, new_time);

                guardian_value_set_int (
                        value,
                        (int)uptime,
                        new_time);
            }
        }
#endif                          /* HAVE_UTMPX_H */

        guardian_item_set_last_update (
                items[ITEM_SYSTEM_UPTIME],
                new_time);
    }
    last_update_time = new_time;
}

static time_t
_get_update_time (void)
{
    return last_update_time;
}
