if not exist "%APPDATA%\PJ64 1.6.2\AUTO0.iso" (
	msg * "You must run these bat files as administrator from right click if you're not using the Manage SD Space option in Project64 1.6.2"
	exit /b
)
pushd "%~dp0"
start /wait "" "%~dp0SdCardMounter.exe"
powershell -Command "Mount-DiskImage -ImagePath '%APPDATA%\PJ64 1.6.2\AUTO0.vhd'"
for /f "tokens=2 delims==" %%D in ('wmic logicaldisk get deviceid /value ^| findstr "DeviceID"') do (
	set "Drive=%%D"
)
pushd %Drive%
if not exist "Mario Builder 64 Levels" mkdir "Mario Builder 64 Levels"
popd
exit /b