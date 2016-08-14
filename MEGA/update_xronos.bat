@echo off
REM =============================================================================
rem === Author: LensDigital (xronosclock.com)
REM === This will update XRONOS Firmware using AVRDude ====
REM === WinAVR Needs to be present ====
REM --- Parameters: 1st COM Port Number, 2nd location and name of firmeware file, 3rd MCU type (644 or 1284)
REM ==============================================================================
REM --- Modify Below variables to correct location ---
REM --- Location of avrdude.exe and config---
SET AVRPath="c:\xronos\avr\bin\avrdude.exe"
SET AVRConfig="c:\xronos\avr\etc\avrdude.conf"
REM === DO NOT MODIFY BELOW

IF "%1"=="" goto MISSINGPARAM
IF "%2"=="" goto MISSINGPARAM
IF "%3"=="" goto MISSINGPARAM

rem --- Check if Files specified exist
IF NOT EXIST %AVRPath% GOTO BADAVR
IF NOT EXIST %AVRConfig% GOTO BADCONF
IF NOT EXIST %2 GOTO BADHEX

rem --- Modify parametesr based on which MCU is used
IF %3%==644 (SET MCU=atmega644p & SET BAUD=57600) ELSE (SET MCU=atmega1284p  & SET BAUD=115200)

rem --- Execute Upload
color 1F
@echo **** Starting Firmware Update. This might take few minutes... 
rem call %AVRPath% -C%AVRConfig% -patmega644p -cstk500v1 -P\\.\COM%1 -b57600 -D -Uflash:w:%2%:i
rem --- Uncomment below for FTDI upload
call %AVRPath% -C%AVRConfig% -p%MCU% -carduino -P\\.\COM%1 -b%BAUD% -D -Uflash:w:%2%:i
rem --- Uncomment below for Arduino as ISP upload
rem call %AVRPath% -C%AVRConfig% -p %MCU% -c avrisp -P COM%1 -b 19200 -v -e -U flash:w:%2%:i
IF %ERRORLEVEL% NEQ 0 GOTO NOTUPDATED

echo SUCCESS! Firmware udpated!
color 2F
GOTO END
:MISSINGPARAM
Color 4F
echo ERROR Missing parameters!
echo Please specify COM port number, firmware filename and MCU type
echo For example:
echo update_xronos.bat xronos1_05.cpp.hex 644
GOTO END
:BADAVR
Color 4F
echo ERROR avrdude.exe not found at specified path:
echo %AVRPath%
GOTO END
:BADCONF
Color 4F
echo ERROR avrdude.conf not found at specified path:
echo %AVRConfig%
GOTO END
:BADHEX
Color 4F
echo ERROR Specified Firemware file not found:
echo %2
GOTO END
:NOTUPDATED
Color 4F
echo Unknown ERROR. Firmware was not uploaded :(
:END
pause