#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <dlfcn.h>


#include <libguardian/libguardian.h>

static int
_timestamp_compare_func (
        const void *a,
        const void *b );

void
libguardian_init ( void )
{
    guardian_field_register (
            "__timestamp",
            _timestamp_compare_func );
            
}

static int
_timestamp_compare_func (
        const void *a,
        const void *b )
{
    return strcmp (a, b);
}
