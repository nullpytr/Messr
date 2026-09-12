/* MSR for Windows, https://github.com/nullpytr/wMSR/blob/4e39e73b9f8428b85e333d01265488e5696d9571/include/msr.hpp */

#ifndef WIN32_H
#define WIN32_H

#include <windows.h>

#define MSR_DEVICE_TYPE 40000
#define IOCTL_READ_MSR  CTL_CODE(MSR_DEVICE_TYPE, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_WRITE_MSR CTL_CODE(MSR_DEVICE_TYPE, 0x801, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define MSR_NT_DEVICE_NAME      L"\\Device\\msr"
#define MSR_DOS_DEVICE_NAME     L"\\DosDevices\\msr"
#define MSR_WIN32_DEVICE_NAME   L"\\\\.\\msr"

typedef unsigned __int32 MSR_DOUBLE;
typedef unsigned __int64 MSR_QUAD;
typedef unsigned __int32 MSR_NO;
typedef unsigned __int32 MSR_CPU;

#pragma warning(push)
#pragma warning(disable: 4201)
typedef struct _MSR_VALUE {
    union {
        struct {
            MSR_DOUBLE l;
            MSR_DOUBLE h;
        };
        MSR_QUAD q;
    };
} MSR_VALUE;
#pragma warning(pop)

typedef struct _MSR_REQUEST {
    MSR_NO    msr_no;
    MSR_CPU   cpu;
    MSR_VALUE val;
} MSR_REQUEST, *PMSR_REQUEST;

#define MSR_INLINE static inline

MSR_INLINE HANDLE msr_open(void) {
    return CreateFileW(
        /* [in] lpFileName            */ MSR_WIN32_DEVICE_NAME,
        /* [in] dwDesiredAccess       */ GENERIC_READ | GENERIC_WRITE,
        /* [in] dwShareMode           */ 0,
        /* [in] lpSecurityAttributes  */ NULL,
        /* [in] dwCreationDisposition */ OPEN_EXISTING,
        /* [in] dwFlagsAndAttributes  */ 0,
        /* [in] hTemplateFile         */ NULL
    );
}

MSR_INLINE void msr_close(HANDLE device) {
    CloseHandle(device);
}

MSR_INLINE BOOL msr_ioctl(HANDLE device, DWORD const control_code, PMSR_REQUEST request) {
    DWORD bytes_returned;
    return DeviceIoControl(
        /* [in ] hDevice          */ device,
        /* [in ] dwIoControlCode  */ control_code,
        /* [in ] lpInBuffer       */ request,
        /* [in ] nInBufferSize    */ sizeof(MSR_REQUEST),
        /* [out] lpOutBuffer      */ request,
        /* [in ] nOutBufferSize   */ sizeof(MSR_REQUEST),
        /* [out] lpBytesReturned  */ &bytes_returned,
        /* [in ] lpOverlapped     */ NULL
    );
}

MSR_INLINE BOOL msr_read(HANDLE device, MSR_CPU cpu, MSR_NO reg, MSR_QUAD *value) {
    MSR_REQUEST request = {
        .msr_no = reg,
        .cpu = cpu
    };
    BOOL result = msr_ioctl(device, IOCTL_READ_MSR, &request);
    if (result) *value = request.val.q;
    return result;
}

MSR_INLINE BOOL msr_write(HANDLE device, MSR_CPU cpu, MSR_NO reg, MSR_QUAD value) {
    MSR_REQUEST request = { 
        .msr_no = reg,
        .cpu = cpu,
        .val = { .q = value }
    };
    return msr_ioctl(device, IOCTL_WRITE_MSR, &request);
}
#undef MSR_INLINE

static HANDLE msr_device = INVALID_HANDLE_VALUE;

#endif // WIN32_H
