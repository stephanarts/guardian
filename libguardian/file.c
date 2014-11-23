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

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>

#include <string.h>

#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif /* HAVE_ZLIB_H */

#ifdef HAVE_BZLIB_H
#include <bzlib.h>
#endif /* HAVE_BZLIB_H */


#include <openssl/sha.h>

#include "memory.h"
#include "error.h"
#include "log.h"
#include "file.h"

struct _GuardianFile
{
    char *path;
    FILE *stream;

    /* Cached values to determine log rotation */
    ino_t st_ino;
    off_t st_pos;
    off_t st_size;
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

    file->stream = NULL;

    return file;
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

    FILE *stream = NULL;

    /**
     * Use a separate stream,
     * file->stream is reserved for read operations.
     */
    stream = fopen(file->path, "r");

    if (stream == NULL)
    {
        error_sv = errno;
        guardian_log_warning (
            "Can not open file: %s:'%s'",
            file->path,
            strerror (error_sv));
        return -1;
    }

    /* Read file statistics */
    fd = fileno (stream);
    fstat (fd, &buffer);

    /* If the file is smaller then expected,
     * it can't be the file we are looking for.
     */
    if (buffer.st_size < st_size)
    {
        fclose(stream);
        return 2;
    }

    SHA1_Init (&sha_ctx);

    while (i < st_size)
    {
        if (i+DATA_BUFFER_SIZE < st_size)
        {
            _size = fread (data_buffer, 1, DATA_BUFFER_SIZE, stream);
        }
        else
        {
            _size = fread (data_buffer, 1, st_size - i, stream);
        }

        SHA1_Update (&sha_ctx, data_buffer, _size);
        i += _size;
    }

    SHA1_Final ((unsigned char *)file_hash, &sha_ctx);

    fclose(stream);

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

    FILE *stream = NULL;
    
    /**
     * Use a separate stream,
     * file->stream is reserved for read operations.
     */
    stream = fopen(file->path, "r");

    if (stream == NULL)
    {
        error_sv = errno;
        guardian_log_warning (
            "Can not open file: %s:'%s'",
            file->path,
            strerror (error_sv));
        return -1;
    }

    /* Read file statistics */
    fd = fileno (stream);
    fstat (fd, &buffer);

    *st_size = buffer.st_size;

    SHA1_Init (&sha_ctx);

    while (i < buffer.st_size)
    {
        if (i+DATA_BUFFER_SIZE < buffer.st_size)
        {
            _size = fread (data_buffer, 1, DATA_BUFFER_SIZE, stream);
        }
        else
        {
            _size = fread (data_buffer, 1, buffer.st_size - i, stream);
        }

        SHA1_Update (&sha_ctx, data_buffer, _size);
        i += _size;
    }

    SHA1_Final ((unsigned char *)(*hash), &sha_ctx);

    fclose(stream);

    return 0;
}

int
guardian_file_read (
        GuardianFile *file,
        size_t size,
        void   *buffer,
        GuardianError **error) {

    int fd;
    struct stat st_buffer;
    size_t s = 0;
    int error_sv;

    if (file->stream == NULL) {
        file->stream = fopen(file->path, "r");
        if (file->stream == NULL) {
            if (error) {
                *error = guardian_error_new (
                    "Can not open file: %s:'%s'",
                    file->path,
                    strerror (error_sv));
            }
            guardian_log_error (
                "Can not open file: %s:'%s'",
                file->path,
                strerror (error_sv));
            return -1;
        }

        fd = fileno (file->stream);
        fstat (fd, &st_buffer);

        file->st_ino  = st_buffer.st_ino;
        file->st_size = st_buffer.st_size;
        file->st_pos  = 0;
    }

    s = fread (buffer, 1, size, file->stream);
    if (s < size) {
        if (feof(file->stream)) {
            clearerr(file->stream);

            fd = open(file->path, O_RDONLY);
            fstat (fd, &st_buffer);
            close(fd);

            if (st_buffer.st_ino != file->st_ino) {
                guardian_log_info ("File INODE changed, log-rotation expected. Opening new stream.");

                fclose(file->stream);

                file->stream = fopen(file->path, "r");
                if (file->stream == NULL) {
                    error_sv = errno;

                    if (error) {
                        *error = guardian_error_new (
                            "Can not open file: %s:'%s'",
                            file->path,
                            strerror (error_sv));
                    }
                    guardian_log_error (
                        "Can not open file: %s:'%s'",
                        file->path,
                        strerror (error_sv));

                    return -1;
                }

                fd = fileno (file->stream);
                fstat (fd, &st_buffer);

                file->st_ino  = st_buffer.st_ino;
                file->st_size = st_buffer.st_size;
                file->st_pos  = 0;
            }
        }
        if (ferror(file->stream)) {
            guardian_log_warning("An error occurred while reading file.");
        }
    }

    return s;
}
