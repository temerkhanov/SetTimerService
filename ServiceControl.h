#ifndef __SERVICE_CONTROL_H__
#define __SERVICE_CONTROL_H__

#include <windows.h>
#include <tchar.h>

DWORD ServiceInstall(LPCTSTR ServiceName,
                     LPCTSTR DisplayName,
                     LPCTSTR ExecPath);

DWORD ServiceUninstall(LPCTSTR ServiceName);

#endif
