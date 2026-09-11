#ifndef COMPAT_SYS_TYPES_H
#define COMPAT_SYS_TYPES_H

#ifndef _WIN32
#include_next <sys/types.h>
#else
#include <stdint.h>
#endif

#endif /* COMPAT_SYS_TYPES_H */
