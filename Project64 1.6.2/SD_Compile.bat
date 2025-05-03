:: Logic in this file should be corrected so that it checks if the name of the drive is SDCARD0. It should check the drives in reverse order, starting with the most recent (highest letter) one. For example if the most recent one is E: drive it should then check the name, if it's not SDCARD0, it should go to D:
pushd "%~dp0"
for /f "tokens=2 delims==" %%D in ('wmic logicaldisk get deviceid /value ^| findstr "DeviceID"') do set "Drive=%%D"
powershell -Command "Get-Volume -DriveLetter '%Drive:~0,1%' | Get-DiskImage | Dismount-DiskImage"
start /wait "" "%~dp0SdCardMounter.exe"
exit /b