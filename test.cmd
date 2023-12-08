::
:: Run the latest test/*main*.exe (by default), whatever flavor it is...
::

@echo off
:!!call %~dp0tooling\_setenv.cmd

set _exe_pattern=*main*exe
set _run_dir=test

for /f %%f in ('dir /b /o-d /t:w "%_run_dir%\%_exe_pattern%"') do (
	set "latest_exe=%%f"
	goto :break
)
:break

echo Launching: %latest_exe% %*...
"%_run_dir%\%latest_exe%" %*
