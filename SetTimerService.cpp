// SetTimerService.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <tchar.h>

#include "NtTimer.h"
#include "Registry.h"
#include "ServiceControl.h"

SERVICE_STATUS        gServiceStatus = { 0 };
SERVICE_STATUS_HANDLE gStatusHandle = NULL;

ULONG                 gOriginalTimerResolution;

VOID WINAPI ServiceMain(DWORD Argc, LPTSTR Argv[]);
VOID WINAPI ServiceCtrlHandler(DWORD);

#define SERVICE_NAME       _T("SetTimer")
#define DISPLAY_NAME       _T("Set Kernel Timer")
#define PARAMETER_NAME     _T("TimerResolution")
#define MAX_TIMER_PERIOD      (100000)
#define MIN_TIMER_PERIOD      (5000)
#define DEFAULT_TIMER_PERIOD  (20000) /* 2 ms */

LONG SetupTimerResolution(VOID)
{
    LONG  Result;
    DWORD TimerResolution;

    Result = RegistryGetServiceParameter(SERVICE_NAME, PARAMETER_NAME, &TimerResolution);

    if ((Result != ERROR_SUCCESS) ||
        (TimerResolution > MAX_TIMER_PERIOD) ||
        (TimerResolution < MIN_TIMER_PERIOD))
    {
        TimerResolution = DEFAULT_TIMER_PERIOD;
    }

    if (Result == ERROR_FILE_NOT_FOUND) {
        Result = RegistrySetServiceParameter(SERVICE_NAME, PARAMETER_NAME, TimerResolution);
    }

    return TimerResolution;
}

VOID WINAPI ServiceMain(DWORD Argc, LPTSTR Argv[])
{
    NTSTATUS Status;
    LONG     TimerResolution;

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

    TimerResolution = SetupTimerResolution();

    Status = NtSetTimerResolution(TimerResolution, TRUE, &gOriginalTimerResolution);

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
    NTSTATUS Status;
    ULONG    TimerResolution;

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

        Status = NtSetTimerResolution(gOriginalTimerResolution, FALSE, &TimerResolution);

        if (SetServiceStatus(gStatusHandle, &gServiceStatus) == FALSE)
        {
            return;
        }

        break;

    default:
        break;
    }

}

#define PARAM_USAGE                          \
    _T("Command line parameters:\r\n")       \
    _T("/INSTALL     install service\r\n")   \
    _T("/UNINSTALL   uninstall service\r\n")

#define PARAM_HELP \
    _T("/? for a list of supported parameters\r\n")

#define PARAM_TOO_MANY \
    _T("Too many command line parameters\r\n")

#define PARAM_UNKNOWN \
    _T("Unknown command line parameter\r\n")

#define PARAM_INSTALL \
    _T("/INSTALL")

#define PARAM_UNINSTALL \
    _T("/UNINSTALL")

#define PARAM_QUESTION \
    _T("/?")

int _tmain(int Argc, LPTSTR Argv[])
{
    UNREFERENCED_PARAMETER(Argc);
    UNREFERENCED_PARAMETER(Argv);

    SERVICE_TABLE_ENTRY ServiceTable[] =
    {
        {SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
        {NULL, NULL}
    };

    if (Argc > 2) {
        _tcprintf(PARAM_TOO_MANY);
        _tcprintf(PARAM_HELP);
        return ERROR_INVALID_PARAMETER;
    } else if (Argc == 2) {
        if (_tcsicmp(Argv[1], PARAM_QUESTION) == 0)
        {
            _tcprintf(PARAM_USAGE);
            return ERROR_SUCCESS;
        }
        else if (_tcsicmp(Argv[1], PARAM_INSTALL) == 0)
        {
            return ServiceInstall(SERVICE_NAME, DISPLAY_NAME, NULL);
        }
        else if (_tcsicmp(Argv[1], PARAM_UNINSTALL) == 0)
        {
            return ServiceUninstall(SERVICE_NAME);
        }
        else
        {
            _tcprintf(PARAM_UNKNOWN);
            _tcprintf(PARAM_HELP);
            return ERROR_INVALID_PARAMETER;
        }
    }

    if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
    {
        return GetLastError();
    }

    return 0;
}
