# SetTimerService
SetTimerService is a tiny Windows NT4/2000/XP compatible service intended to fix distorted sound in VMWare guests. This is done by setting the kernel timer resolution early during the bootup process.

Installation:

Copy the SetTimerService.exe binary to a permanent location (e.g. C:\Windows\System32)

In Windows command prompt change directory to the location where the binary is located.

"SetTimerService /install" - install SetTimerService

Uninstall:

"SetTimerService /uninstall" - uninstall SetTimerService

install_xp.cmd - adds SetTimerService as a dependency for Windows sound system, should prevent possible races at startup.

Tuning:
Registry values are located at HKLM:SYSTEM\CurrentControlSet\Services\SetTimerService\Parameters

TimerResolution: Dword - Desired timer resolution in 100 ns quanta.
