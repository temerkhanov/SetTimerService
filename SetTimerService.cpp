// SetTimerService.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <tchar.h>
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

SERVICE_STATUS        gServiceStatus = { 0 };
SERVICE_STATUS_HANDLE gStatusHandle = NULL;

ULONG                 gOriginalTimerResolution;

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv);
VOID WINAPI ServiceCtrlHandler(DWORD);

#define SERVICE_NAME  _T("SetTimerService")

int _tmain(int argc, TCHAR* argv[])
{
    SERVICE_TABLE_ENTRY ServiceTable[] =
    {
        {SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
        {NULL, NULL}
    };

    if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
    {
        return GetLastError();
    }

    return 0;
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv)
{
    NTSTATUS Status;

    gStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

    if (gStatusHandle == NULL)
    {
         goto Exit;
    }

    // Prepare to start
    ZeroMemory(&gServiceStatus, sizeof(gServiceStatus));

    gServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gServiceStatus.dwControlsAccepted = 0;
    gServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    gServiceStatus.dwWin32ExitCode = 0;
    gServiceStatus.dwServiceSpecificExitCode = 0;
    gServiceStatus.dwCheckPoint = 0;

    if (SetServiceStatus(gStatusHandle, &gServiceStatus) == FALSE)
    {
        goto Exit;
    }

    Status = NtSetTimerResolution(20000, TRUE, &gOriginalTimerResolution);

    if (Status == STATUS_TIMER_RESOLUTION_NOT_SET) {
        gServiceStatus.dwControlsAccepted = 0;
        gServiceStatus.dwCurrentState = SERVICE_STOPPED;
        gServiceStatus.dwWin32ExitCode = Status;
    } else {
        // Set the running status
        gServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
        gServiceStatus.dwCurrentState = SERVICE_RUNNING;
        gServiceStatus.dwWin32ExitCode = 0;
    }

    gServiceStatus.dwCheckPoint = 1;

    if (SetServiceStatus(gStatusHandle, &gServiceStatus) == FALSE)
    {
        goto Exit;
    }

Exit:
    return;
}

VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
    switch (CtrlCode)
    {
    case SERVICE_CONTROL_STOP:
        if (gServiceStatus.dwCurrentState != SERVICE_RUNNING)
            break;

        /*
         * Stop the service and cleanup here
         */

        gServiceStatus.dwControlsAccepted = 0;
        gServiceStatus.dwCurrentState = SERVICE_STOPPED;
        gServiceStatus.dwWin32ExitCode = 0;
        gServiceStatus.dwCheckPoint = 4;

        if (SetServiceStatus(gStatusHandle, &gServiceStatus) == FALSE)
        {
            return;
        }

        break;

    default:
        break;
    }

}
