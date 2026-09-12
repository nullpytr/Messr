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

    if (!msr_read(msr_device, cpu, reg, data))
        return 0;

    return size;
}

static size_t pwrite(int fd, const void *data, size_t size, off_t offset) {
    uint32_t cpu = (uint32_t)fd;
    uint32_t reg = (uint32_t)offset;

    if (!msr_write(msr_device, cpu, reg, *(const MSR_QUAD *)data))
        return 0;
        
    return size;
}

static void close(int _) { }

#undef exit
static void compat_exit(int code) {
    ExitProcess(code ? 1 : 0);
}
#define exit compat_exit

#undef perror
static void compat_perror(const char *s) {
    DWORD err = GetLastError();
    char *buf = NULL;

    if (err == ERROR_FILE_NOT_FOUND) {
        buf = "Could not find the wMSR driver.\r\nSee https://github.com/nullpytr/wMSR for help.\r\n";
    } else {
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
            NULL, err, 0, (LPSTR)&buf, 0, NULL
        );
    }

    fprintf(stderr, "%s: %s", s, buf ? buf : "unknown error");
    // LocalFree(buf);
    
    ExitProcess(err);
}
#define perror compat_perror

#endif /* WIN32 */

#endif /* COMPAT_UNISTD_H */
