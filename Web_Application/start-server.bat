@echo off

:: Kill all processes using port 3000
echo Killing any process using port 3000...
for /f "tokens=5" %%a in ('netstat -ano ^| findstr :3000') do taskkill /PID %%a /F >nul 2>&1
echo Process killed.

:: Navigate to Robot_Controls folder and run Python script
cd Robot_Controls
start /B python experimental_send_code.py
set "PYTHON_PID=%!"

:: Check if Camo client is already running
tasklist /FI "IMAGENAME eq CamoStudio.exe" | find /I "CamoStudio.exe" >nul
if %errorlevel% == 0 (
    echo Camo client is already running.
) else (
    echo Starting Camo client...
    start /MIN "Camo Studio" "%~dp0Camo Studio/CamoStudio.exe"
    set "CAMO_PID=%!"

    :: Wait for a few seconds to ensure the client has started
    timeout /t 5 /nobreak >nul
)

:: Check if wireless ADB is connected
@REM echo Checking ADB connection...
@REM cd /d "%~dp0platform-tools"
@REM adb connect 192.168.0.100:5555 >nul 2>&1
@REM adb devices | findstr "192.168.0.100:5555" >nul
@REM if %errorlevel% == 0 (
@REM     echo Wireless ADB is connected.
@REM ) else (
@REM     echo Wireless ADB is not connected. Attempting to reconnect via USB...
@REM     adb usb >nul 2>&1
@REM     adb tcpip 5555
@REM     adb connect 192.168.0.100:5555
@REM     if %errorlevel% == 0 (
@REM         echo Wireless ADB connection established.
@REM     ) else (
@REM         echo Could not establish wireless ADB connection. Please ensure the device is connected via USB first.
@REM         exit /b
@REM     )
@REM )

@REM :: Send ADB commands to wake up the device and start Camo
@REM adb shell input keyevent KEYCODE_WAKEUP
@REM adb shell monkey -p com.reincubate.camo 1

:: Start the Node.js server in the background and capture the process ID (PID)
cd ..
cd Web_Application
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
@REM taskkill /PID %CAMO_PID% /F >nul 2>&1

:: Terminate the Python process if it's still running
taskkill /PID %PYTHON_PID% /F >nul 2>&1

:: Navigate to the Android home screen and put the device to sleep
@REM adb shell input keyevent KEYCODE_HOME
@REM adb shell input keyevent KEYCODE_SLEEP

@REM :: Disconnect from the Android device
@REM adb disconnect 192.168.0.100:5555

exit /b
