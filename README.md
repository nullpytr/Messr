# Messr

A fully featured cross-platform port of `rdmsr`/`wrmsr` from [intel/msr-tools](https://github.com/intel/msr-tools). Windows support added via the [wMSR](https://github.com/nullpytr/wMSR) kernel driver. 

The upstream source files are unchanged, compatibility headers (`include/compat`) bridge the Linux kernel device (`/dev/cpu/N/msr`) interface to the wMSR driver interface at compile time.

## Requirements

**Windows**: the `msr.sys` kernel driver from [wMSR](https://github.com/nullpytr/wMSR) must be loaded before running either tool.

**Linux**: the `msr` kernel module must be loaded (`modprobe msr`).

## Usage

### rdmsr

```
Usage: rdmsr [options] regno
  --help         -h  Print this help
  --version      -V  Print current version
  --hexadecimal  -x  Hexadecimal output (lower case)
  --capital-hex  -X  Hexadecimal output (upper case)
  --decimal      -d  Signed decimal output
  --unsigned     -u  Unsigned decimal output
  --octal        -o  Octal output
  --c-language   -c  Format output as a C language constant
  --zero-pad     -0  Output leading zeroes
  --raw          -r  Raw binary output
  --all          -a  all processors
  --processor #  -p  Select processor number (default 0)
  --bitfield h:l -f  Output bits [h:l] only
```

### wrmsr

```
Usage: wrmsr [options] regno value...
  --help         -h  Print this help
  --version      -V  Print current version
  --all          -a  all processors
  --processor #  -p  Select processor number (default 0)
```

## Build for Windows 

Output at `build/win64/rdmsr.exe` and `build/win64/wrmsr.exe`.

### 1. Native

Requires MSVC. Run from a [Developer Command Prompt](https://learn.microsoft.com/en-us/visualstudio/ide/reference/command-prompt-powershell).
```cmd
scripts\build
```

### 2. Cross-compile from macOS or Linux

Requires [mingw-w64](https://www.mingw-w64.org). On macOS: 
```
brew install mingw-w64
```

Run from any terminal:
```sh
scripts/build.sh win64
```


## Build for Linux

Output at `build/native/rdmsr` and `build/native/wrmsr`.

```sh
scripts/build.sh native
```

## How it works

Both `rdmsr.c` and `wrmsr.c` are unmodified from the upstream [intel/msr-tools](https://github.com/intel/msr-tools) source. 

On Linux they directly use the kernel's `/dev/cpu/N/msr` devices via standard POSIX calls. On Windows, the `include/compat/` headers provide a fake posix API to `rdmsr.c`/`wrmsr.c` and redirect the syscalls to the [wMSR](https://github.com/nullpytr/wMSR) kernel driver.

This works because the compat headers are included before the system headers, on Linux they simply passthrough to system headers while on windows they implement the fake posix API.

```c
#ifndef _WIN32
#include_next <posix_header.h>
#else
// fake windows-only API
#endif
```

### Device open

On Linux, each CPU is a separate device file. `open` returns a file descriptor and the CPU index is baked into the path:

```c
sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
fd = open(msr_file_name, O_RDONLY);
```

On Windows, there is one device (`\\.\msr`) shared for all CPUs. The device handle is stored in a `static` variable in `win32.h`, shared across all compat headers:

```c
static HANDLE msr_device = INVALID_HANDLE_VALUE;
```

The compat `open()` function parses the CPU index out of the path, opens the wMSR device once (lazily, on first call), and returns the CPU index instead of the fd.  


```c
static int open(const char *name, int _) {
    if (msr_device == INVALID_HANDLE_VALUE)
        msr_device = msr_open();

    int cpu;
    sscanf(name, "/dev/cpu/%d/msr", &cpu);
    return cpu;
}
```

### Register read/write

On Linux, `pread` uses the file offset as the MSR register number:

```c
p[read | write](fd, &data, sizeof data, reg);
```

On Windows, the compat `p[read | write]()` functions reinterpret the fake fd (returned by the compat `open()` function) as the CPU index and the offset as the register number, then call into the wMSR kernel driver:

```c
static size_t p[read | write](int fd, void *data, size_t size, off_t offset) {
    uint32_t cpu = (uint32_t)fd;
    uint32_t reg = (uint32_t)offset;

    if (![msr_read | msr_write](msr_device, cpu, reg, data))
        return 0;

    return size;
}
```

### CPU enumeration

On Linux, CPUs are enumerated by scanning `/dev/cpu/`:

```c
scandir("/dev/cpu", &namelist, dir_filter, 0);
```

On Windows, `GetActiveProcessorCount()` gives the count and the compat `scandir()` function fakes the directory entries:

```c
static int scandir(..., struct dirent ***namelist, ...) {
    DWORD count = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
    for (DWORD p = 0; p < count; ++p) {
        int len = snprintf(NULL, 0, "%u", (unsigned)p);
        struct dirent *entry = malloc(sizeof(struct dirent) + len + 1);
        sprintf(entry->d_name, "%u", (unsigned)p);
        ...
    }
}
```

### Error reporting

On Linux, the upstream code checks specific posix `errno` values (`ENXIO`, `EIO`) to distinguish error types before falling through to generic `perror()`.  

On Windows, these error paths are not possible, so we define the POSIX signals as ULONG_MAX (which will never match any `errno` value), forcing all error paths to the compat `perror()` function:

```c
#ifndef ENXIO
#define ENXIO ULONG_MAX
#endif

#ifndef EIO
#define EIO   ULONG_MAX
#endif
```

And the compat `perror()` function prints the human-readable error message returned by the windows kernel API and immediately exits, with a special case for `ERROR_FILE_NOT_FOUND` to point the user at the wMSR kernel driver:

```c
static void compat_perror(const char *s) {
    DWORD err = GetLastError();
    char *buf = NULL;

    if (err == ERROR_FILE_NOT_FOUND) {
        buf = "Could not find the wMSR driver. See https://github.com/nullpytr/wMSR";
    } else {
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | ..., NULL, err, 0, &buf, 0, NULL);
    }
    fprintf(stderr, "%s: %s", s, buf);

    ExitProcess(err);
}
```

### Device close

On Linux, `close(fd)` releases the file descriptor after each read or write.

On Windows, the static device handle `msr_device` lives for the lifetime of the process. So the compat `close()` function is a no-op and the handle is only closed by the OS when the process exits:

```c
static void close(int _) { }
```