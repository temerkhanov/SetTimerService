// SetTimerService.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <tchar.h>

#include "NtTimer.h"

SERVICE_STATUS        gServiceStatus = { 0 };
SERVICE_STATUS_HANDLE gStatusHandle = NULL;

ULONG                 gOriginalTimerResolution;

VOID WINAPI ServiceMain(DWORD Argc, LPTSTR* Argv);
VOID WINAPI ServiceCtrlHandler(DWORD);

#define SERVICE_NAME  _T("SetTimerService")

VOID WINAPI ServiceMain(DWORD Argc, LPTSTR* Argv)
{
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Argc);
    UNREFERENCED_PARAMETER(Argv);

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

int _tmain(int Argc, TCHAR* Argv[])
{
    UNREFERENCED_PARAMETER(Argc);
    UNREFERENCED_PARAMETER(Argv);

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
