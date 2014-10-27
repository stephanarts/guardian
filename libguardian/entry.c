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

#include <openssl/sha.h>


#include "error.h"
#include "entry.h"

struct _GuardianEntry
{
    /**
     * SHA1 hash of the length field ((4 bytes) in Network byte order),
     * and the entry-data.
     */
    unsigned char   hash[20];   

    /**
     * Timestamp value in ISO format
     *
     * ZULU Time:
     *   YYYYMMDDThhmmssZ
     * Timezone correction:
     *   YYYYMMDDThhmmss+hhmm
     *   YYYYMMDDThhmmss-hhmm
     *
     * Value is either 16 or 20 octets long.
     * The value if timestamp[15] determines
     * if the remaining 4 bytes contain any 
     * data (hhmm, or nothing)
     * Checking for 'Z', '+' or '-' is just 
     * as expensive as checking for '\0'.
     */
    char            timestamp[20];

    /** Line-Nr in the original file */
    size_t          line_nr;

    /** Length of the data-field    */
    size_t          len;

    /** The entry data              */
    char           *data;
};

GuardianEntry *
guardian_entry_new (
        size_t          line_nr,
        size_t          len,
        const char     *data,
        const char     *timestamp,
        GuardianError **error)
{
    GuardianEntry *entry = (GuardianEntry *)malloc (sizeof (GuardianEntry));

    SHA_CTX context;
    size_t n = htonl(len);
    size_t s = htonl(line_nr);

    SHA1_Init (&context);
    SHA1_Update (&context, &n, sizeof(size_t));
    SHA1_Update (&context, &s, sizeof(size_t));
    SHA1_Update (&context, data, len);
    SHA1_Final (entry->hash, &context);

    entry->line_nr = line_nr;
    entry->len = len;

    entry->data = (char *)malloc(len);
    strncpy (entry->data, data, len);

    strncpy (entry->timestamp, timestamp, sizeof(entry->timestamp));

    return entry;
}
