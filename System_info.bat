@echo off
echo ==========================================
echo       System Information Script
echo ==========================================
echo.


echo [1] Operating System version:
systeminfo | findstr /B /C:"OS Name" /C:"OS Version"
echo.

echo [2] Memory (RAM) Information:
wmic MEMORYCHIP get Capacity,Manufacturer,Speed
echo.

echo [3] CPU Information:
wmic cpu get Name, NumberOfCores, MaxClockSpeed
echo.

echo [4] Network Configuration:
ipconfig /all | findstr /C:"IPv4 Address" /C:"Default Gateway" /C:"Subnet Mask"
echo.

echo [5] Disk Space Information:
wmic logicaldisk get deviceid,volumename,size,freespace
echo.

echo ===========================================
echo         System Check Complete
echo ===========================================
pause