#ifndef __NT_TIMER_H__
#define __NT_TIMER_H__

#include <winnt.h>

#define STATUS_SUCCESS 0
#define STATUS_TIMER_RESOLUTION_NOT_SET 0xC0000245

#ifndef NTSTATUS
typedef LONG NTSTATUS, *PNTSTATUS;
#endif

EXTERN_C NTSYSAPI
NTSTATUS
NTAPI
NtSetTimerResolution(
    IN ULONG                DesiredResolution,
    IN BOOLEAN              SetResolution,
    OUT PULONG              CurrentResolution);

#endif