pushd "%~dp0"
start /wait "" "%~dp0SD_Mount.exe"
powershell -Command "Mount-DiskImage -ImagePath '%APPDATA%\PJ64 1.6.2\AUTO0.vhd'"
for /f "tokens=2 delims==" %%D in ('wmic logicaldisk get deviceid /value ^| findstr "DeviceID"') do set "Drive=%%D"
pushd %Drive%
if not exist "Mario Builder 64 Levels" mkdir "Mario Builder 64 Levels"
popd
exit /b