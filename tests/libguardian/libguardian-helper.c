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

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <libguardian/libguardian.h>

#ifndef PLUGINDIR
#define PLUGINDIR "../../plugins"
#endif

#ifndef PLUGINSUBDIR
#define PLUGINSUBDIR ".libs/"
#endif

#define BUFFER_SIZE 1024

#define POS_GMT_OFFSET 120
#define NEG_GMT_OFFSET -120

enum {
    OPTION_VERSION = 0,
    OPTION_HELP,
    OPTION_FILE_VERIFY,
    OPTION_SOURCE_READ,
    OPTION_COUNT
};

/************************
 * Command-line options *
 ************************/
static struct option long_options[] = {
    {"version",    0, 0, 'V'}, /* OPTION_VERSION */
    {"help",       0, 0, 'h'}, /* OPTION_HELP */
    {"file-verify",0, 0, 0},   /* OPTION_FILE_VERIFY */
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
    printf ("Usage: %s [options]\n"
            "    [--file-verify <file> <hash>]\n"
            "    [--source-read <def>]\n"
            , PACKAGE_NAME);
    printf ("\n");
    printf ("Options:\n");
    printf ("   --version  -V     Show version information\n");
    printf ("   --help     -h     Show usage information (this output)\n");
    printf ("\n");
    printf ("   --file-verify     Test file-verify feature\n");
    printf ("   --source-read     Test source functionality\n");
    return;
}

int
main (int argc, char **argv)
{
    int option_index = 0;
    int c = 0;

    int file_verify = 0;
    int source_read = 0;

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
                    case OPTION_FILE_VERIFY:
                        file_verify = 1;
                        break;
                    case OPTION_SOURCE_READ:
                        source_read = 1;
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

    libguardian_init();

    guardian_log_init (1);
    guardian_log_mask (GUARDIAN_LOG_DEBUG);

    if (file_verify == 1) {
        //fprintf(stderr, "Verify file: %i", argc);
        //if (argc != 3)
        //   exit(1); 
        GuardianFile *f = guardian_file_new(argv[2]);

        for(c = 0; c < argc; ++c) {
            //fprintf(stderr, "%d: %s\n", c, argv[c]);
        }

        if (strlen(argv[4]) != 40)
           exit(1); 

        /* Convert ASCII SHA sum to binary 20-byte string */
        unsigned char hash[20];
        for(c = 0; c < 20; ++c) {
            unsigned int n;
            sscanf(&argv[4][c*2], "%2x", &n);
            hash[c] = (unsigned char)n;
        }

        int i = guardian_file_verify (f, atoi(argv[3]), hash);

        //fprintf(stderr, "%d\n", i);
    }

    //guardian_set_timezone_gmt_offset(POS_GMT_OFFSET);
    //guardian_set_timezone_gmt_offset(NEG_GMT_OFFSET);

    
    exit(0);
}
