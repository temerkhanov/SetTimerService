#include "ServiceControl.h"
#include <windows.h>

DWORD ServiceInstall(LPCTSTR ServiceName,
                     LPCTSTR DisplayName,
                     LPCTSTR ExecPath)
{
    SC_HANDLE SCManagerHandle;
    SC_HANDLE ServiceHandle;
    TCHAR     Path[MAX_PATH];
    DWORD     Result;

    if (ExecPath == NULL)
    {
        if (!GetModuleFileName(NULL, Path, MAX_PATH))
        {
            return GetLastError();
        }
    }

    SCManagerHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (NULL == SCManagerHandle)
    {
        return GetLastError();
    }

    ServiceHandle = CreateService(
        SCManagerHandle,
        ServiceName,
        DisplayName,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        Path,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);

    Result = GetLastError();

    if (NULL == ServiceHandle)
    {
        goto Exit;
    }

    CloseServiceHandle(ServiceHandle);

Exit:
    CloseServiceHandle(SCManagerHandle);

    return Result;
}

DWORD ServiceUninstall(LPCTSTR ServiceName)
{
    SC_HANDLE SCManagerHandle;
    SC_HANDLE ServiceHandle;
    DWORD     Result;

    SCManagerHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (NULL == SCManagerHandle)
    {
        return GetLastError();
    }

    ServiceHandle = OpenService(
        SCManagerHandle,
        ServiceName,
        SERVICE_ALL_ACCESS);

    if (NULL == ServiceHandle)
    {
        Result = GetLastError();
        goto Exit;
    }

    DeleteService(ServiceHandle);

    Result = GetLastError();

    CloseServiceHandle(ServiceHandle);

Exit:
    CloseServiceHandle(SCManagerHandle);

    return Result;
}
