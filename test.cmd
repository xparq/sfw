::
:: Run the latest default test, or the one matching the pattern on the cmdline
::

@echo off
::!! No explicit prj-level setup needed yet (miraculously), but that might change any day:
::!! call %~dp0tooling\setenv.cmd

if     "%1" == "" set "_exe_pattern=*main*.exe"
if not "%1" == "" (
	set "_exe_pattern=*%1*"
	shift
)

set run_dir=test

for /f %%f in ('dir /b /o-d /t:w "%run_dir%\%_exe_pattern%"') do (
	set "_latest_matching=%%f"
	goto :break
)
:break

if "%_latest_matching%" == "" (
	echo - ERROR: "%run_dir%\%_exe_pattern%" not found!
	goto :eof
)

echo Launching: %_latest_matching% %1 %2 %3 %4 %5 %6 %7 %8 %9
"%run_dir%\%_latest_matching%"  %1 %2 %3 %4 %5 %6 %7 %8 %9
