setlocal enabledelayedexpansion
pushd "%~dp0"
for /f "tokens=1 delims=:" %%D in ('wmic logicaldisk get deviceid ^| findstr ":" ^| sort /R') do (
set "DriveLetter=%%D"
for /f %%L in ('powershell -NoProfile -Command "(Get-Volume -DriveLetter '!DriveLetter!').FileSystemLabel"') do (
if /i "%%L"=="SDCARD0" (
powershell -Command "Get-Volume -DriveLetter '!DriveLetter!' | Get-DiskImage | Dismount-DiskImage"
start /wait "" "%~dp0SD_Mount.exe"
exit /b
)
)
)
exit /b