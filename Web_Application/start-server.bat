@echo off

:: Check if Camo client is already running
tasklist /FI "IMAGENAME eq CamoStudio.exe" | find /I "CamoStudio.exe" >nul
if %errorlevel% == 0 (
    echo Camo client is already running.
) else (
    echo Starting Camo client...
    start /MIN "Camo Studio" "%~dp0Camo Studo/CamoStudio.exe"
    set "CAMO_PID=%!"
    
    :: Wait for a few seconds to ensure the client has started
    timeout /t 5 /nobreak >nul
)

cd /d "%~dp0platform-tools"
adb connect 192.168.1.100:5555
adb shell input keyevent KEYCODE_WAKEUP

:: Start the Camo app on your Android device
adb shell monkey -p com.reincubate.camo 1

:: Start the Node.js server in the background and capture the process ID (PID)
cd ..
start /B node server.js
set "NODE_PID=%!"

:: Define the URL to open
set "URL=http://localhost:3000"

:: Search for available browsers and open the URL
echo Searching for browsers...

:: Check for Google Chrome
where /R "C:\Program Files (x86)" chrome.exe >nul 2>&1
if %errorlevel% == 0 (
    echo Opening URL in Google Chrome...
    start chrome %URL%
) else (
    echo Chrome not found.
)

:: Check for Mozilla Firefox
where /R "C:\Program Files (x86)" firefox.exe >nul 2>&1
if %errorlevel% == 0 (
    echo Opening URL in Mozilla Firefox...
    start firefox %URL%
) else (
    echo Firefox not found.
)

:: Check for Microsoft Edge
where /R "C:\Program Files (x86)" msedge.exe >nul 2>&1
if %errorlevel% == 0 (
    echo Opening URL in Microsoft Edge...
    start msedge %URL%
) else (
    echo Microsoft Edge not found.
)

:: If no browsers found, print a message
if %errorlevel% neq 0 (
    echo No compatible web browser found to open the URL.
)

:waitloop
:: Wait for the terminal to close or Node.js server to stop
ping -n 5 127.0.0.1 >nul
tasklist /FI "PID eq %NODE_PID%" 2>NUL | find /I "node.exe" >NUL
if %errorlevel% == 0 goto waitloop

:cleanup
echo "Cleaning up..."
:: Terminate the Node.js server process if it's still running
taskkill /PID %NODE_PID% /F >nul 2>&1

:: Terminate the Camo client process if it's still running
taskkill /PID %CAMO_PID% /F >nul 2>&1

cd "platform-tools"
adb disconnect 192.168.1.100:5555
exit /b

