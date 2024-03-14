::
:: In POSIX-ish environments, where GNU make, sh, find etc. are on the PATH,
:: just run `make`. On Windows, though, this script might spare some nuisances.
::

@echo off
PATH=%~dP0tooling;%~dP0tooling\build;%PATH%

make %*
