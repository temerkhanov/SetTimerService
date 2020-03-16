#ifndef __REGISTRY_H__
#define __REGISTRY_H__

#include <windows.h>

LONG RegistryGetServiceParameter(
    LPCTSTR ServiceName,
    LPCTSTR ParameterName,
    LPDWORD ParameterValue
    );

LONG RegistrySetServiceParameter(
    LPCTSTR ServiceName,
    LPCTSTR ParameterName,
    DWORD   ParameterValue
    );

#endif