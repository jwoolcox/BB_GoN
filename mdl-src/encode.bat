@echo off

@echo Changing directory to %~dp0
cd %~dp0

@echo Encoding *.dae: 

for /r %%i in (*.dae) do gameplay-encoder.exe -v 4 %%i
IF %ERRORLEVEL% NEQ 0 pause

for /r %%i in (*.ttf) do gameplay-encoder.exe -v 4 -s 20 -p %%i
IF %ERRORLEVEL% NEQ 0 pause

@echo Moving files to resources
move /Y *.gpb ..\res\gpb\

IF %ERRORLEVEL% NEQ 0 pause