#!/bin/sh

# To avoid collision on Windows
FIND=/usr/bin/find

dir=${1:-.}
echo Counting lines in `realpath $dir`:

# The strange full path is just for me (or those who also have it this way), 
# to disambiguate from Win's FIND.EXE in a mixed env.:
#$FIND $dir -type f -a \( -name '*.sh' -o -name '*.cmd' -o -name '*akefile*' -o -name 'run_*' \) -a -exec wc -l \{\} \; \
#	| awk '{ total += $1 } END{ print total }'
#exit $?

#!!!
#!! WOW, these used to differ quite a bit at Space Test, on Windows, who knows why!
#!! E.g. BB couldn't find build-gcc.sh, defs.sh etc.! :-ooooo
#!!!

#echo "With $FIND:"
#$FIND $dir -type f -a \( \
#		-name '*.cpp' -o \
#		-name '*.hpp' -o \
#		-name '*.hh'  -o \
#		-name '*.h'  -o \
#		-name '*.cc'  -o \
#		-name '*.ixx' -o \
#		-name '*.inl' -o \
#		-name '*.inc' \
#		\) -a -exec wc -l \{\} \; \
#	| awk '{ total += $1 } END{ print total }'

#echo "With BusyBox find:"
busybox find $dir -type f -a \( \
		-name '*.cpp' -o \
		-name '*.hpp' -o \
		-name '*.hh'  -o \
		-name '*.h'  -o \
		-name '*.cc'  -o \
		-name '*.ixx' -o \
		-name '*.inl' -o \
		-name '*.inc' \
		\) -a -exec wc -l \{\} \; \
	| awk '{ total += $1 } END{ print total }'
