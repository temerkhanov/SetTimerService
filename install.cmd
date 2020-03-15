sc create SetTimer binPath= "%CD%\SetTimerService.exe" DisplayName= "Set Kernel Timer" start= auto 
sc config AudioSrv depend= "PlugPlay/RpcSs/SetTimer"