#ifndef COMPAT_SYS_TYPES_H
#define COMPAT_SYS_TYPES_H

#ifndef _WIN32
#include_next <sys/types.h>
#else

#include <stdint.h>

#ifndef _OFF_T_DEFINED
typedef int64_t off_t;
#define _OFF_T_DEFINED
#endif

#endif /* WIN32 */

#endif /* COMPAT_SYS_TYPES_H */
