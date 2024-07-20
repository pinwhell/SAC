@echo off

REM Run the PowerShell command
powershell -NoProfile -Command "Get-Content -Path %SAC_LOGPATH_LOCAL% -Wait"