@echo off

::!! Can't just do a nonchalant SETLOCAL ENABLEEXTENSIONS here, because any
::!! variables set here will not be visible by callers, EVEN IF INVOKED BY
::!! `call _setenv`! :-/


pushd %~dP0..
set "PRJ_ROOT_ABS=%CD%"
popd

set "PRJ_ROOT_ABS_=%PRJ_ROOT_ABS%\"


:: --- DEBUG ---
goto :eof
echo %PRJ_ROOT_ABS%
echo %PRJ_ROOT_ABS_%

