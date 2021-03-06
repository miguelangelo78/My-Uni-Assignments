@cd "%~dp0"
@echo off

SET PATH=%Toolchain%;%Util%;%PATH%

@cd ..\..\..\Build

@make -j all DEFINE_CAR_YEAR=-define=CAR_YEAR=2018

@cd "%~dp0"

SET fdt_exe="C:\Program Files (x86)\Renesas\FDT4.09\fdt.exe"
SET fdt_script=fdt_workspace_2018\fdt_script.fsf

IF [%1]==[] GOTO NOARG

@echo workspace "%~dp0fdt_workspace_2018\fdt_workspace.aws">%fdt_script%
@echo disconnect reset>>%fdt_script%
@echo connect com11>>%fdt_script%
@echo download %1^|user>>%fdt_script%
@echo disconnect reset>>%fdt_script%
@echo exit >> fdt_workspace_2018\fdt_script2.fsf>>%fdt_script%

@echo.
@echo - Flashing program ...

@%fdt_exe% /DISCRETESTARTUP "w4fRun \"fdt_workspace_2018\fdt_script.w4f\""

@echo - Done flashing

GOTO DONE

:NOARG

@echo ERROR: Did not specify .mot file
@echo Usage: burn.bat ^<.mot file^>

:DONE