/*-
 * Copyright (c) 2012-2014 Stephan Arts. All Rights Reserved.
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

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <signal.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/un.h>

#include <dirent.h>

#include <pthread.h>

#include <unistd.h>
#include <string.h>

#include <openssl/sha.h>

#include <zmq.h>

#include <time.h>

#include <libguardian/libguardian.h>

#include "assert.h"
#include "util.h"
#include "settings.h"
#include "scheduler.h"
#include "db.h"

#ifndef MAX_PLUGINS
#define MAX_PLUGINS 10
#endif

static GuardianPlugin *_plugins[MAX_PLUGINS];
static int n_plugins = 0;

static GuardianPluginDB *_db_plugin = NULL;

static int _init_db = 0;

enum
{
    OPTION_VERSION = 0,
    OPTION_VERBOSE,
    OPTION_HELP,
    OPTION_FATAL_WARNINGS,
    OPTION_INIT_DB
};

/************************
 * Command-line options *
 ************************/
static struct option long_options[] = {
    {"version", 0, 0, 'V'},     /* OPTION_VERSION */
    {"verbose", 0, 0, 'v'},     /* OPTION_VERBOSE */
    {"help", 0, 0, 'h'},        /* OPTION_HELP    */
    {"fatal-warnings", 0, 0, 0},/* OPTION_FATAL_WARNINGS */
    {"init-db", 0, 0, 0},       /* OPTION_INIT_DB */
    {0, 0, 0, 0}
};

static void
show_version ()
{
    printf ("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
    printf ("Copyright (c) 2012-2014 Stephan Arts\n");
    printf ("There is NO WARRANTY, to the extent permitted by law.\n");
    return;
}

static void
show_usage ()
{
    printf ("Usage: %s [options]\n", PACKAGE_NAME);
    printf ("\n");
    printf ("Options:\n");
    printf ("   --version  -V     Show version information\n");
    printf ("   --help     -h     Show usage information (this output)\n");
    printf ("              -v     Show verbose output\n");
    printf ("              -vv    Show very-verbose output\n");
    printf ("\n");
    printf ("   --init-db         Initialize DB schema\n");
    printf ("\n");
    printf ("   --fatal-warnings  Make all warnings fatal\n");
    return;
}

void
process_signal (int s)
{
    switch (s)
    {
    case SIGINT:
        /**
         * Let's ignore further instances of SIGINT.
         */
        signal (SIGINT, SIG_IGN);
        guardian_scheduler_main_quit ();
        break;
    case SIGTERM:
        guardian_scheduler_main_quit ();
        break;
    default:
        break;
    }
    return;
}

/**
 * main
 * @argc: Number of elements in argv
 * @argv: Array of command-line options provided to the application
 *
 * @returns: Exit-code
 */
int
main (int argc, char **argv)
{
    int     option_index = 0;
    int     c = 0;
    int     verbosity = 0;
    int     log_level = 0;
    GuardianError *error = NULL;

    DIR    *plugin_dir = NULL;
    struct dirent *dirp;
    int     i = 0;

    int     n_workers = 2;

    char    plugin_path[1024];

    struct sigaction sa;

    void   *ctx = zmq_ctx_new ();
    void   *host_ptr = NULL;
    void   *ns_ptr = NULL;

    sa.sa_handler = process_signal;
    sa.sa_flags = SA_RESTART;
    sigemptyset (&sa.sa_mask);

    sigaction (SIGINT, &sa, NULL);
    sigaction (SIGTERM, &sa, NULL);

    GuardianSettings *settings = NULL;

    while (1)
    {
        c = getopt_long (argc, argv, "vVh",
                long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
        case 0:
            switch (option_index)
            {
            case OPTION_VERSION:
                show_version ();
                exit (0);
                break;
            case OPTION_HELP:
                show_usage ();
                exit (0);
                break;
            case OPTION_FATAL_WARNINGS:
                guardian_set_fatal_asserts (TRUE);
                break;
            case OPTION_INIT_DB:
                _init_db = 1;
                printf("_INITDB_\n");
                break;
            }
            break;
        case 'V':
            show_version ();
            exit (0);
            break;
        case 'h':
            show_usage ();
            exit (0);
            break;
        case 'v':
            verbosity = verbosity + 1;
            break;
        default:
            fprintf (stderr, "Try '%s --help' for more information\n", PACKAGE_NAME);
            exit (1);
            break;
        }
    }

    if (verbosity == 0)
    {

#ifdef ENABLE_DEBUG
        guardian_log_init (1);
#else
        guardian_log_init (0);
#endif                          /* ENABLE_DEBUG */

        switch (log_level)
        {
        default:

#ifdef ENABLE_DEBUG
            guardian_log_mask (GUARDIAN_LOG_DEBUG);
            guardian_log_info ("Set logging level to 'DEBUG'");
#else
            guardian_log_mask (log_level);
            guardian_log_info ("Set logging level to '...'");
#endif                          /* ENABLE_DEBUG */
        }
    } else
    {
        guardian_log_init (1);

        switch (verbosity)
        {
        case 1:
            guardian_log_mask (GUARDIAN_LOG_INFO);
            guardian_log_info ("Set logging level to 'INFO'");
            break;
        case 2:
            guardian_log_mask (GUARDIAN_LOG_DEBUG);
            guardian_log_info ("Set logging level to 'DEBUG'");
            break;
        }
    }

    /**
     * Initialise guardian library
     */
    libguardian_init ();

    /**
     * Load settings from file
     */
    settings = guardian_settings_load (SYSCONFDIR "/guardian.conf", NULL);

    char *db_type = guardian_settings_get (settings, "db_type");
    if (db_type == NULL) {
        guardian_log_error ("DB-Type not specified in config-file");
        exit(1);
    }

    /**
     * Maximum 10 items (Development value).
     */
    guardian_items_init (10);

    /**
     * Load all plugins from PLUGINDIR
     */
    plugin_dir = opendir (PLUGINDIR);
    if (plugin_dir != NULL)
    {
        while ((dirp = readdir (plugin_dir)) != NULL)
        {
            if (n_plugins == MAX_PLUGINS)
            {
                guardian_log_warning (
                        "Max plugins reached (%d), "
                        "can not load more plugins.");
                break;
            }
            i = snprintf (plugin_path, 1024, "%s/%s", PLUGINDIR, dirp->d_name);
            if (i < 0)
            {
                /* An error occurred */
            } else
            {
                if (i > 1023)
                {
                    /* Error, prevented buffer overflow... path too long */
                    guardian_log_warning (
                            "Can not load plugin, "
                            "plugin-path exceeds 1024 bytes");
                } else
                {
                    /**
                     * Only load .so files, the rest can't be a plugin.
                     */
                    if (strcmp (&plugin_path[i - 3], ".so") == 0)
                    {
                        _plugins[n_plugins] = guardian_plugin_load (
                                plugin_path,
                                &error);

                        if (_plugins[n_plugins] == NULL && error)
                        {
                            guardian_log_warning (
                                    "%s",
                                    guardian_error_get_msg (error));
                            guardian_error_free (error);
                            error = NULL;
                        } else
                        {
                            guardian_log_info (
                                    "Load plugin: %s\n",
                                    plugin_path);
                            //guardian_plugin_register_types (plugin);
                            n_plugins++;
                        }
                    }
                }
            }
        }
    }

#ifdef ENABLE_DEBUG
    /** Do not allow further dynamic memory allocation */
    guardian_set_allow_malloc (FALSE);
#endif                          /* ENABLE_DEBUG */

    for (i = 0; i < n_plugins; ++i)
    {
        if (_plugins[i]->type == GUARDIAN_PLUGIN_DB)
        {
            if (strcmp(
                    ((GuardianPluginDB *)_plugins[i])->db_name,
                    db_type) == 0)
            {
                if (_db_plugin != NULL) {
                    printf("Duplicate DB-Types\n");
                    exit(1);
                }

                _db_plugin = (GuardianPluginDB *)_plugins[i];
            }
        }
    }

    if (_db_plugin == NULL)
    {
        printf("Failed to initialize database '%s', no such plugin\n", db_type);
        exit(1);
    }

    char **keys;
    int l = _db_plugin->db.listprop(&keys);
    for (i = 0; i < l; ++i) {
        char *val = guardian_settings_get (settings, keys[i]);
        if (val != NULL) {
            _db_plugin->db.setprop(keys[i], val);
        }
    }

    if (_init_db != 0) {
        char *p;
        _db_plugin->db.getprop("db_schema", &p);
        printf(">>> %s\n", p);
        if (_db_plugin->db.init(&error) != 0)
        {
            printf("%s", guardian_error_get_msg(error));
        }
        exit(0);
    }

    _db_plugin->db.connect(NULL);

    _db_plugin->host.add(
            "aa",
            &error);

    _db_plugin->host.get(
            "aa",
            &host_ptr,
            &error);

    _db_plugin->ns.add(
            "sys",
            host_ptr,
            &error);


    /** Start the main loop */
    guardian_scheduler_main (ctx, n_workers);

    _db_plugin->db.disconnect(NULL);

    //zmq_ctx_term (ctx);

    exit (0);
}

#if 0
                ((GuardianPluginDB *)_plugins[i])->db.connect();
    
                ((GuardianPluginDB *)_plugins[i])->host.get(
                        "aa",
                        &host_ptr,
                        &error);

                host_ptr = NULL;

                ((GuardianPluginDB *)_plugins[i])->host.get(
                        "aa",
                        &host_ptr,
                        &error);

                ((GuardianPluginDB *)_plugins[i])->ns.add(
                        "sys",
                        host_ptr,
                        &error);

                ((GuardianPluginDB *)_plugins[i])->ns.get(
                        "sys",
                        host_ptr,
                        &ns_ptr,
                        &error);

                if (error != NULL)
                {
                    guardian_log_warning (
                            "Add: %s\n",
                            guardian_error_get_msg (error));
                }

                /*
                ((GuardianPluginDB *)_plugins[i])->ns.list(
                        "aa",
                        NULL,
                        NULL,
                        &error);
                */

                /*
                ((GuardianPluginDB *)_plugins[i])->metric.add(
                        ns_ptr,
                        "cpu.load.avg[1]",
                        &error);

                ((GuardianPluginDB *)_plugins[i])->metric.add(
                        ns_ptr,
                        "cpu.load.avg[5]",
                        &error);

                ((GuardianPluginDB *)_plugins[i])->metric.add(
                        ns_ptr,
                        "cpu.load.avg[15]",
                        &error);
                */

                ((GuardianPluginDB *)_plugins[i])->db.disconnect();
                break;

#endif
