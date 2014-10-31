/*-
 * Copyright (c) 2012 Stephan Arts. All Rights Reserved.  *
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

/*
 * This application is used by the various plugin test-scripts as a framework
 * for testing the various plugins.
 */

#include <config.h>

#include <stdlib.h>

#include <stdio.h>

#include <string.h>

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <libguardian/libguardian.h>

struct _GuardianFile
{
    char *path;
    FILE *stream;

    /* Cached values to determine log rotation */
    ino_t st_ino;
    off_t st_pos;
    off_t st_size;
};

enum {
    OPTION_VERSION = 0,
    OPTION_HELP,
    OPTION_TEST_VERIFY,
    OPTION_TEST_READ,
    OPTION_COUNT
};

/************************
 * Command-line options *
 ************************/
static struct option long_options[] = {
    {"version",     0, 0, 'V'}, /* OPTION_VERSION */
    {"help",        0, 0, 'h'}, /* OPTION_HELP */
    {"test-verify", 0, 0, 0},   /* OPTION_TEST_VERIFY */
    {"test-read",   0, 0, 0},   /* OPTION_TEST_READ */
    {0, 0, 0, 0}
};

static void
show_version ()
{
    printf ("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
    printf ("Copyright (c) 2014 Stephan Arts\n");
    printf ("There is NO WARRANTY, to the extent permitted by law.\n");
    return;
}

static void
show_usage ()
{
    printf ("Usage: %s [options]\n",
            PACKAGE_NAME);
    printf ("\n");
    printf ("Options:\n");
    printf ("   --version  -V     Show version information\n");
    printf ("   --help     -h     Show usage information (this output)\n");
    printf ("\n");
    printf ("   --test-verify     Verify file hash (SHA)\n");
    return;
}

int
main (int argc, char **argv)
{
    int option_index = 0;
    int c = 0;
    int ret = 0;

    struct stat buffer;
    int fd;

    int _test_verify = 0;
    int _test_read = 0;

    char data_buffer[1024];

    while (1)
    {
        c = getopt_long (argc, argv, "Vh",
                    long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
            case 0:
                switch (option_index)
                {
                    case OPTION_VERSION:
                        show_version();
                        exit(0);
                        break;
                    case OPTION_HELP:
                        show_usage();
                        exit(0);
                        break;
                    case OPTION_TEST_VERIFY:
                        _test_verify = 1;
                        break;
                    case OPTION_TEST_READ:
                        _test_read = 1;
                        break;
                }
                break;
            case 'V':
                show_version ();
                exit(0);
                break;
            case 'h':
                show_usage();
                exit(0);
                break;
            default:
                fprintf(stderr, "Try '%s --help' for more information\n", argv[0]);
                exit (1);
                break;
        }
    }

    argc-=optind;
    argv+=optind;

    libguardian_init();

    if (_test_verify == 1) {

        if (argc < 2)
           exit(1); 

        if (strlen(argv[1]) != 40)
           exit(1); 

        GuardianFile *f = guardian_file_new(argv[0]);

        /* Convert 40-byte ASCII SHA sum to binary 20-byte string */
        unsigned char hash[20];
        for(c = 0; c < 20; ++c) {
            unsigned int n;
            sscanf(&argv[1][c*2], "%2x", &n);
            hash[c] = (unsigned char)n;
        }

        /* If no file-size is specified, check the whole file */
        if (argc != 3) {
            fd = open (argv[0], O_RDONLY);
            fstat (fd, &buffer);
            close(fd);
        
            ret = guardian_file_verify (f, buffer.st_size, hash);

        } else {
            ret = guardian_file_verify (f, atoi(argv[2]), hash);
        }

        exit(ret);
    }


    if (_test_read == 1) {
        GuardianError *err = NULL;
        GuardianFile *f = guardian_file_new(argv[0]);

        int i = guardian_file_read (f,
            1024,
            &data_buffer,
            NULL);

        ino_t st_ino = f->st_ino;

        rename(argv[0], argv[1]);

        int fd = open(argv[0], O_CREAT|O_WRONLY, 0600);
        write(fd, "1243\n", 5);
        close(fd);

        /* Determine EOF and discover rotation */
        int a = guardian_file_read (f,
            1024-i,
            &data_buffer[i],
            NULL);

        if (a > 0)
        {
            ret = 1;
            exit(ret);
        }
        
        /* Read file */
        i = guardian_file_read (f,
            1024-i,
            &data_buffer[i],
            &err);

        if (i != 5) {
            fprintf(stderr, "Read %d bytes, expected 5\n", i);
            if (err) {
                fprintf(stderr, "%s", guardian_error_get_msg(err));
            }
            ret = 2;
            exit(ret);
        }

        if (f->st_ino == st_ino) {
            ret = 1;
            exit(ret);
        }

        printf("%s\n", data_buffer);

        exit(ret);
    }
}
