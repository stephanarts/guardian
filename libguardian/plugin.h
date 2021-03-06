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

#ifndef __GUARDIAN_PLUGIN_H__
#define __GUARDIAN_PLUGIN_H__


typedef struct _GuardianPlugin GuardianPlugin;

struct _GuardianPlugin
{
    void *handle; /* dlopen handle */
    void (*register_types) ( GuardianPlugin * );
    void (*extract_fields) ( GuardianPlugin *, const char *entry );
};

GuardianPlugin *
guardian_plugin_load ( 
        char *path,
        GuardianError **);

void
guardian_plugin_push_entry (
        GuardianPlugin *,
        const char *host, 
        const char *source,
        const char *timestamp,
        const char *entry);

void
guardian_plugin_register_types ( GuardianPlugin * );

void
guardian_plugin_extract_fields (
        GuardianPlugin *,
        const char *entry );


#endif /* __GUARDIAN_PLUGIN_H__ */
