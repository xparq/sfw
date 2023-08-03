#!/bin/sh

export DISPLAY=:0
export LIBGL_ALWAYS_INDIRECT=0

exe="${1:-sfw-demo}"

# mounted_prjdir=/mnt/c/sz/prj/sfw
mounted_prjdir=`pwd`

cd "$mounted_prjdir" && export LD_LIBRARY_PATH="${mounted_prjdir}/extern/sfml/linux/lib" && "${mounted_prjdir}/$exe"
