#ifndef COMPAT_FCNTL_H
#define COMPAT_FCNTL_H

#ifndef _WIN32
#include_next <fcntl.h>
#else

#include <stdio.h>
#include <limits.h>
#include <win32.h>

static int open(const char *name, int _) {
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

#ifndef O_RDONLY
#define O_RDONLY 0
#endif
#ifndef O_WRONLY
#define O_WRONLY 1
#endif

#ifndef ENXIO
#define ENXIO ULONG_MAX
#endif
#ifndef EIO
#define EIO   ULONG_MAX
#endif

#endif /* WIN32 */

#endif /* COMPAT_FCNTL_H */
