@echo off
color 1F
rem ===========================================================================
rem === Author: LensDigital (xronosclock.com)
rem ==== INSTRUCTIONS =====
rem ---- Change value of FWVER to version of firmware (i.e. 2_06)
rem ---- (FIRMWARE) filename of the HEX firmware file with file location (path) ----
rem ===========================================================================
SET FWVER=R1
SET ATMEGA=1284
rem ============================================================================
rem ++++ DO NOT MODIFY LINE BELOW!!!! +++++
rem ============================================================================
:START
cls
@echo =======================================================
@echo --- ATMega1284p Compatible Xronos firmware updater ----
@echo =======================================================
SET /P COMPORT=Enter COM port number (enter 0 to exit) and press ENTER:
IF %COMPORT%==0 GOTO EXIT
:UPDATE
rem SET FIRMEWARE=xronos%FWVER%_%ATMEGA%.hex
@echo Ready to Upload %CD%\xronos_mega_%FWVER%.hex via COM%COMPORT% 
pause
call update_xronos.bat %COMPORT% %CD%\xronos_mega_%FWVER%.hex %ATMEGA%
@echo 
goto exit
:HELP
cls
rem === This is no longer used ====
echo ===============================================
echo Your Xronos Clock type is based on 
echo Microprocessor type (ATMega644p or ATMega1284p)
echo and Hardware features such as:
echo - RFM12B Radio Module for external sensors
echo - Infrared Module and Remote
echo To find out which type your clock has look at the
echo bottom for a model number sticker: XCVER-TYPE)
echo where VER=Version number such as 201
echo TYPE is either Microprocessor such as 1284 or 644 
echo or if Microprocessor model omitted it's always 1284 but with following features:
echo RF - RMF12B module
echo IR - Infrared module
echo For example: XC201-RF is Xronos Clock Ver 2.01
echo with ATMega1284p MCU and RFM12B module installed, but no IR
echo If lable is missing or you still need help visit:
echo www.xronosclock.com and go to Help/Support section
echo ===============================================
SET /P HELPMENU=Type 1 to go back to main menu, 0 to exit and press ENTER:
IF %HELPMENU%==1 GOTO START
:EXIT