:: WSL GUI (X) executables can't just be run directly from the WSL shell. :-/
:: See: https://github.com/xparq/sfw/issues/15

@echo off
setlocal enableextensions enabledelayedexpansion

set "exe=%1" & if "!exe!" == "" set exe=sfw-demo

set mounted_prjdir=/mnt/c/sz/prj/sfw

wsl -d Debian bash --login -c "cd %mounted_prjdir% && export LD_LIBRARY_PATH=%mounted_prjdir%/extern/sfml/linux/lib && ./%exe%"
