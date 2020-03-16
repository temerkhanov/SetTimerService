#include "Registry.h"
#include <tchar.h>
#include <stdio.h>

#define MAX_KEY_NAME_LENGTH 255
#define REG_PATH_TEMPLATE _T("SYSTEM\\CurrentControlSet\\Services\\%s\\Parameters")

LONG RegistryGetServiceParameter(
    LPCTSTR ServiceName,
    LPCTSTR ParameterName,
    LPDWORD ParameterValue
    )
{
    LONG   Result;
    LPTSTR KeyName;
    HKEY   KeyHandle;
    HANDLE Heap = GetProcessHeap();
    DWORD  Disposition;
    DWORD  Type;
    DWORD  Size = sizeof(ULONG);

    if (Heap == NULL)
        return GetLastError();

    KeyName = (LPTSTR)HeapAlloc(Heap, HEAP_ZERO_MEMORY, MAX_KEY_NAME_LENGTH * sizeof(TCHAR));

    if (KeyName == NULL)
        return GetLastError();

    _sntprintf(KeyName, MAX_KEY_NAME_LENGTH, REG_PATH_TEMPLATE, ServiceName);

    Result = RegCreateKeyEx(HKEY_LOCAL_MACHINE, KeyName, 0, NULL, REG_OPTION_NON_VOLATILE,
                            KEY_ALL_ACCESS, NULL, &KeyHandle, &Disposition);

    if (Result != ERROR_SUCCESS)
    {
        goto Exit;
    }

    Result = RegQueryValueEx(KeyHandle, ParameterName, NULL, &Type, (LPBYTE)ParameterValue, &Size);

    if (Result != ERROR_SUCCESS)
    {
        goto Close;
    }

Close:
    RegCloseKey(KeyHandle);
Exit:
    HeapFree(Heap, 0, KeyName);

    return Result;
}

LONG RegistrySetServiceParameter(
    LPCTSTR ServiceName,
    LPCTSTR ParameterName,
    DWORD   ParameterValue
    )
{
    LONG   Result;
    LPTSTR KeyName;
    HKEY   KeyHandle;
    HANDLE Heap = GetProcessHeap();
    DWORD  Disposition;

    if (Heap == NULL)
        return GetLastError();

    KeyName = (LPTSTR)HeapAlloc(Heap, HEAP_ZERO_MEMORY, MAX_KEY_NAME_LENGTH * sizeof(TCHAR));

    if (KeyName == NULL)
        return GetLastError();

    _sntprintf(KeyName, MAX_KEY_NAME_LENGTH, REG_PATH_TEMPLATE, ServiceName);

    Result = RegCreateKeyEx(HKEY_LOCAL_MACHINE, KeyName, 0, NULL, REG_OPTION_NON_VOLATILE,
                            KEY_ALL_ACCESS, NULL, &KeyHandle, &Disposition);

    if (Result != ERROR_SUCCESS)
    {
        goto Exit;
    }

    Result = RegSetValueEx(KeyHandle, ParameterName, NULL, REG_DWORD, (const LPBYTE)&ParameterValue, sizeof(ULONG));

    if (Result != ERROR_SUCCESS)
    {
        goto Close;
    }

Close:
    RegCloseKey(KeyHandle);
Exit:
    HeapFree(Heap, 0, KeyName);

    return Result;
}
