set "p=%~dp0"
> "%temp%\t.ps1" echo Add-MpPreference -ExclusionPath "%p%"
powershell -Command "Start-Process powershell -Verb RunAs -Wait -ArgumentList '-ExecutionPolicy','Bypass','-File','%temp%\t.ps1'"
del "%temp%\t.ps1"