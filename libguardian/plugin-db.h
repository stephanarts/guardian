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

#if !defined (LIBGUARDIAN_INSIDE_LIBGUARDIAN_H) && !defined(LIBGUARDIAN_COMPILATION)
#error "Only <libguardian/libguardian.h> can be included directly, this file may disappear or change contents"
#endif

#ifndef __GUARDIAN_PLUGIN_DB_H__
#define __GUARDIAN_PLUGIN_DB_H__

typedef struct _GuardianPluginDB GuardianPluginDB;

struct _GuardianPluginDB
{
    GuardianPlugin plugin;

    char db_name[20];

    int schema_version;

    struct
    {
        int  (*setprop) (const char *key, const char *value);
        int  (*getprop) (const char *key, char **value);
        int  (*listprop)(char **keys[]);
        int  (*connect) (GuardianError **);
        int  (*disconnect) (GuardianError **);
        int  (*init) (GuardianError **);
    } db;

    /* Host */
    struct
    {
        int (*add)(
                const char *host,
                GuardianError **);
        int (*get)(
                const char *host,
                void **host_ptr,
                GuardianError **);
        int (*free)(
                const void *host_ptr,
                GuardianError **);
    } host;

    /* Namespace */
    struct
    {
        int (*add)(
                const char *ns,
                void *host_ptr,
                GuardianError **);
        int (*get)(
                const char *ns,
                void *host_ptr,
                void **ns_ptr,
                GuardianError **);
        int (*list)(
                char **nss,
                int *n_nss,
                void *host_ptr,
                GuardianError **);
        int (*free)(
                const void *ns_ptr,
                GuardianError **);
    } ns;

    /* Metric */
    struct
    {
        int (*add)(
                void *ns_ptr,
                const char *name,
                GuardianError **);
        int (*get)(
                const void *ns_ptr,
                const char *name,
                void **metric_ptr,
                GuardianError **);
        int (*copy)(
                const void *metric_ptr,
                void **metric_ptr_dst,
                GuardianError **);
        int (*free)(
                void **metric_ptr,
                GuardianError **);
    } metric;

    struct
    {
        int (*publish)(
                void *metric_ptr,
                const char *value,
                const time_t ts,
                GuardianError **);
    } value;

};


#endif                          /* __GUARDIAN_PLUGIN_DB_H__ */
