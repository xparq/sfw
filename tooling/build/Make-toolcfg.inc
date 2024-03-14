#-----------------------------------------------------------------------------
# Assume either Windows or (often incorrectly...) Linux for the build env.:
BUILDENV_OS ?= $(if $(or $(shell echo $(windir)),$(shell echo $(WINDIR))),$(if $(or $(shell echo $(VCToolsVersion)),$(shell echo $(VCToolsVersion))),windows,windows),linux)
#$(info BUILDENV_OS: $(BUILDENV_OS))

#-----------------------------------------------------------------------------
# "Detect" default GCC/CLANG flavor ("mingw" on Windows, "linux" otherwise;
# rather hamfisted, but OK for now) -- use `make TOOLCHAIN=...` to override!):
TOOLCHAIN ?= $(if $(or $(shell echo $(windir)),$(shell echo $(WINDIR))),$(if $(or $(shell echo $(VCToolsVersion)),$(shell echo $(VCToolsVersion))),msvc,mingw),linux)

$(info Assuming TOOLCHAIN = $(TOOLCHAIN) ($(BUILDENV_OS)))


#-----------------------------------------------------------------------------
# Tool config...
#

ifeq "$(BUILDENV_OS)" "windows"
#!!$(info BUILDENV_OS: $(BUILDENV_OS))
#!!Doesn't seem to be picked up by make: SHELL := busybox
ECHO  := busybox echo
# Note: CD should be replaced too, for the occasional / vs. \ mismatches...
CD    := cd #!! `busybox cd` -> "applet not found", while e.g. `busybox echo` is fine!... :-o :-/
MKDIR := busybox mkdir -p
FIND  := busybox find
else
ECHO  := echo
CD    := cd
MKDIR := mkdir -p
FIND  := find
endif

# Compilers...
CC.linux       = clang
CXX.linux      = clang++ # Or e.g. g++-13 (for Ubuntu 22)...
CC.mingw       = gcc
CXX.mingw      = g++
CC.msvc        = cl
CXX.msvc       = cl
CC  = ${CC.$(TOOLCHAIN)}
CXX = ${CXX.$(TOOLCHAIN)}
#$(info - Using compiler for C: $(CC), C++: $(CXX))

_compflags_warnings.gcclike := -Wall -pedantic -Wextra -Wshadow -Wwrite-strings
CFLAGS_COMMON = -I. -Iinclude
CFLAGS_COMMON.linux = -o "$@" -MMD $(_compflags_warnings.gcclike) -O3 -fPIC
CFLAGS_COMMON.mingw = -o "$@" -MMD $(_compflags_warnings.gcclike) -O3
CFLAGS_COMMON.msvc  = -Fo"$@" -nologo -W4 -O2
CFLAGS_COMMON.msvc.static =
CFLAGS_COMMON.msvc.shared =
CFLAGS_COMMON.msvc += ${CFLAGS_COMMON.msvc.$(LIB_MODE)}

CFLAGS_COMMON.linux.debug_0 = -DNDEBUG
CFLAGS_COMMON.linux.debug_1 = -DDEBUG -g
CFLAGS_COMMON.mingw.debug_0 = -DNDEBUG
CFLAGS_COMMON.mingw.debug_1 = -DDEBUG -g
CFLAGS_COMMON.msvc.debug_0 = -DNDEBUG
CFLAGS_COMMON.msvc.debug_1 = -DDEBUG -Zi

CFLAGS_COMMON += ${CFLAGS_COMMON.$(TOOLCHAIN)} 
CFLAGS_COMMON += ${CFLAGS_COMMON.$(TOOLCHAIN).debug_$(DEBUG)} 

CFLAGS.linux =
CFLAGS.mingw =
CFLAGS += $(CFLAGS_COMMON) ${CFLAGS.$(TOOLCHAIN)}

CXXFLAGS.linux  = -std=c++23
CXXFLAGS.mingw  = -std=c++23
CXXFLAGS.msvc   = -std:c++latest -EHsc -Zc:preprocessor
CXXFLAGS += $(CFLAGS_COMMON) ${CXXFLAGS.$(TOOLCHAIN)}

# Compiler options to use only when compiling objects for the lib:
CLIBFLAGS.linux  =
CLIBFLAGS.mingw  =
CLIBFLAGS.msvc.static = -Zl # Omit default lib
CLIBFLAGS.msvc.shared = -LD # Create DLL <-!! Likely irrelevant here, as it's a flag for linking!
CLIBFLAGS.msvc   = ${CLIBFLAGS.msvc.$(LIB_MODE)}
CLIBFLAGS = ${CLIBFLAGS.$(TOOLCHAIN)}

# Compiler options for compiling & linking the tests/examples:
CEXEFLAGS.linux  =
CEXEFLAGS.mingw  =
CEXEFLAGS.msvc.debug_0  = -MT
CEXEFLAGS.msvc.debug_1  = -MTd
CEXEFLAGS.msvc = ${CEXEFLAGS.msvc.debug_$(DEBUG)}
CEXEFLAGS = ${CEXEFLAGS.$(TOOLCHAIN)}

# Lib builder
# - shared:
LIBTOOL_SHARED.linux  = $(CXX)
LIBTOOL_SHARED.mingw  = $(CXX)
LIBTOOL_SHARED.msvc   = lib
LIBTOOL_SHARED = ${LIBTOOL_SHARED.$(TOOLCHAIN)}
# + flags:
LIBTOOL_SHARED_FLAGS.linux   = -shared -o "$@"
LIBTOOL_SHARED_FLAGS.mingw   = -shared -o "$@"
LIBTOOL_SHARED_FLAGS.msvc    = -nologo -out:"$@"
LIBTOOL_SHARED_FLAGS = ${LIBTOOL_SHARED_FLAGS.$(TOOLCHAIN)}
# - static:
LIBTOOL_STATIC.linux  = ar
LIBTOOL_STATIC.mingw  = ar
LIBTOOL_STATIC.msvc   = lib
LIBTOOL_STATIC = ${LIBTOOL_STATIC.$(TOOLCHAIN)}
# + flags
LIBTOOL_STATIC_FLAGS.linux   = crvf -o "$@"
LIBTOOL_STATIC_FLAGS.mingw   = crvf -o "$@"
LIBTOOL_STATIC_FLAGS.msvc    = -nologo -out:"$@"
LIBTOOL_STATIC_FLAGS = ${LIBTOOL_STATIC_FLAGS.$(TOOLCHAIN)}

# Linker
LINKER.linux  = $(CXX)
LINKER.mingw  = $(CXX)
LINKER.msvc   = link
LINKER = ${LINKER.$(TOOLCHAIN)}
# - flags
LDFLAGS.linux   = -pthread -o "$@"
LDFLAGS.mingw   = -pthread -o "$@"
LDFLAGS.msvc    = -nologo -out:"$@"
LDFLAGS = ${LDFLAGS.$(TOOLCHAIN)}


#-----------------------------------------------------------------------------
# Src -> obj mapping, platform-specific filename suffix/prefix adjustments...
#

objext.msvc = .obj
objext.linux = .o
objext.mingw = .o
objext = ${objext.$(TOOLCHAIN)}

libext_static.msvc = .lib
libext_static.linux = .a
libext_static.mingw = .a
libext_static = ${libext_static.$(TOOLCHAIN)}
libext_shared.msvc = .dll
libext_shared.linux = .so
libext_shared.mingw = .so
libext_shared = ${libext_shared.$(TOOLCHAIN)}

libname_prefix.msvc =
libname_prefix.linux = lib
libname_prefix.mingw = lib
libname_prefix = ${libname_prefix.$(TOOLCHAIN)}

exeext.msvc  = .exe
exeext.linux =
exeext.mingw = .exe
exeext = ${exeext.$(TOOLCHAIN)}

