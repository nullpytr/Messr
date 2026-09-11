#ifndef COMPAT_UNISTD_H
#define COMPAT_UNISTD_H

#ifndef _WIN32
#include_next <unistd.h>
#else

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <win32.h>

static size_t pread(int fd, void *data, size_t size, off_t offset) {
    uint32_t cpu = (uint32_t)fd;
    uint32_t reg = (uint32_t)offset;

    if (!msr_read(msr_device, reg, cpu, data))
        return 0;

    return size;
}

static size_t pwrite(int fd, const void *data, size_t size, off_t offset) {
    uint32_t cpu = (uint32_t)fd;
    uint32_t reg = (uint32_t)offset;

    if (!msr_write(msr_device, reg, *(const MSR_QUAD *)data, cpu))
        return 0;
        
    return size;
}

static void close(int fd) { }

#endif /* WIN32 */

#endif /* COMPAT_UNISTD_H */
