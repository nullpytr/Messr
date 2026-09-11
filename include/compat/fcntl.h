#ifndef COMPAT_FCNTL_H
#define COMPAT_FCNTL_H

#ifndef _WIN32
#include_next <fcntl.h>
#else

#include <stdio.h>
#include <limits.h>
#include <win32.h>

static HANDLE msr_device = INVALID_HANDLE_VALUE;

static int open(const char *name, int flags) {
    if (msr_device == INVALID_HANDLE_VALUE) {
        msr_device = msr_open();
        if (msr_device == INVALID_HANDLE_VALUE)
            return -1;
    }

    int cpu;
    if (sscanf(name, "/dev/cpu/%d/msr", &cpu) != 1)
        return -1;

    return cpu;
}

#undef O_RDONLY
#undef O_WRONLY

#define O_RDONLY 0
#define O_WRONLY 1

#undef errno
#undef ENXIO
#undef EIO

#define errno   GetLastError()
#define ENXIO   ULONG_MAX
#define EIO     ULONG_MAX

#endif /* WIN32 */

#endif /* COMPAT_FCNTL_H */
