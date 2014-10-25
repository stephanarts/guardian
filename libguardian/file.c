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

#include <errno.h>
#include <sys/stat.h>

#include <stdio.h>

#include <string.h>


#include <openssl/sha.h>

#include "memory.h"
#include "log.h"
#include "file.h"

struct _GuardianFile
{
    char *path;
    FILE *file;
};


/** Allow DATA_BUFFER_SIZE to be overridden with compile-flags */
#ifndef DATA_BUFFER_SIZE
#define DATA_BUFFER_SIZE 1024
#endif

GuardianFile *
guardian_file_new(const char *path)
{
    GuardianFile *file = (GuardianFile *)guardian_new(1, sizeof(GuardianFile));

    file->path = guardian_new(1, strlen(path)+1);
    strncpy(file->path, path, strlen(path));

    file->file = NULL;

    return file;
}

int
guardian_file_open (GuardianFile *file)
{
    file->file = fopen (file->path, "r");
    return 0;
}

int
guardian_file_close (GuardianFile *file)
{
    fclose(file->file);
    file->file = NULL;
    return 0;
}

int
guardian_file_seek (
        GuardianFile *file,
        size_t pos)
{
    if (file->file == NULL) {
        return 1;
    }

    fseeko ( file->file, pos, SEEK_SET);
    return 0;
}

int
guardian_file_verify (
        GuardianFile *file,
        size_t st_size,
        const unsigned char *hash)
{
    SHA_CTX sha_ctx;

    int fd;
    struct stat buffer;
    int error_sv;

    char data_buffer[DATA_BUFFER_SIZE];
    char *str_ptr = &data_buffer[0];
    char *end_ptr = &data_buffer[DATA_BUFFER_SIZE-1];
    char *ptr;

    unsigned char file_hash[20];
    size_t _size;
    int i = 0;

    if (file->file == NULL)
    {
        error_sv = errno;
        guardian_log_warning (
            "Can not open file: %s:'%s'",
            file->path,
            strerror (error_sv));
        return -1;
    }

    /* Read file statistics */
    fd = fileno (file->file);
    fstat (fd, &buffer);

    /* If the file is smaller then expected,
     * it can't be the file we are looking for.
     */
    if (buffer.st_size < st_size)
    {
        return 2;
    }

    SHA1_Init (&sha_ctx);

    while (i < st_size)
    {
        if (i+DATA_BUFFER_SIZE < st_size)
        {
            _size = fread (data_buffer, 1, DATA_BUFFER_SIZE, file->file);
        }
        else
        {
            _size = fread (data_buffer, 1, st_size - i, file->file);
        }

        SHA1_Update (&sha_ctx, data_buffer, _size);
        i += _size;
    }

    SHA1_Final ((unsigned char *)file_hash, &sha_ctx);

    if(memcmp(file_hash, hash, 20))
    {
        return 3;
    }

    return 0;
}

int
guardian_file_get_hash (
        GuardianFile *file,
        size_t *st_size,
        unsigned char **hash)
{
    SHA_CTX sha_ctx;

    int fd;
    struct stat buffer;
    int error_sv;

    char data_buffer[DATA_BUFFER_SIZE];

    int i = 0;
    size_t _size;

    if (file->file == NULL)
    {
        error_sv = errno;
        guardian_log_warning (
            "Can not open file: %s:'%s'",
            file->path,
            strerror (error_sv));
        return -1;
    }

    /* Read file statistics */
    fd = fileno (file->file);
    fstat (fd, &buffer);

    *st_size = buffer.st_size;

    SHA1_Init (&sha_ctx);

    while (i < buffer.st_size)
    {
        if (i+DATA_BUFFER_SIZE < buffer.st_size)
        {
            _size = fread (data_buffer, 1, DATA_BUFFER_SIZE, file->file);
        }
        else
        {
            _size = fread (data_buffer, 1, buffer.st_size - i, file->file);
        }

        SHA1_Update (&sha_ctx, data_buffer, _size);
        i += _size;
    }

    SHA1_Final ((unsigned char *)(*hash), &sha_ctx);

    return 0;
}

int
guardian_file_read (
        GuardianFile *file,
        size_t size,
        size_t nmemb,
        void   *buffer) {
    return 0;
}
