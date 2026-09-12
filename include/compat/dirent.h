#ifndef COMPAT_DIRENT_H
#define COMPAT_DIRENT_H

#ifndef _WIN32
#include_next <dirent.h>
#else

#include <stdio.h>
#include <stdlib.h>
#include <win32.h>

struct dirent {
    char d_name[16];
};

static int scandir(
    const char *__,
    struct dirent ***namelist,
    int (*filter)(const struct dirent *),
    int (*_)(const struct dirent **, const struct dirent **)
) {
    DWORD count = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);

    struct dirent **list = (struct dirent **)malloc(count * sizeof(struct dirent *));
    if (!list) return -1;

    int n = 0;
    for (DWORD p = 0; p < count; ++p) {
        struct dirent *entry = (struct dirent *)malloc(sizeof(struct dirent));
        if (!entry) continue;
        sprintf(entry->d_name, "%u", (unsigned)p);

        if (!filter || filter(entry))
            list[n++] = entry;
        else
            free(entry);
    }

    *namelist = list;
    return n;
}

#endif /* WIN32 */

#endif /* COMPAT_DIRENT_H */
