#-----------------------------------------------------------------------------
# GNU Makefile for GCC/CLANG -- on both "native GNU" (i.e. Linux etc.) and
# Windows (i.e. MinGW, w64devkit or compatible)
#
# Dependencies: 
# - GNU make 4.3 (4.4 if .WAIT is used; not available on Ubuntu 22!)
# - SFML (pre-3.0 "master"): should be preinstalled locally (i.e. manually,
#   or with the help of tooling/sfml-setup (which is for Linux only))
# - OpenGL & other platform deps (see AUXLIBS): should be provided by the host
#   system (e.g. the GHA job installs them before launching the build)
# - freetype: provided by the host for shared builds, or bundled with SFML
#   for static linking [!! Mentioned separately from the rest because I had
#   issues with shared-linking it on a WSL Debian, but can't recall what exactly...]
#-----------------------------------------------------------------------------

include tooling/build/Make-toolcfg.inc

#-----------------------------------------------------------------------------
# Build options...
#
# - LIB_MODE: static (shared (would mean DLL on windows) not yet supported)
# - DEBUG: 0 (default) or 1 (also selects the SFML debug libs if 1!)
# - CC: g++ (default) or clang++
# - SFML_DIR: pre-built SFML package dir for the current toolchain
# - SFML_LINKMODE: static (default for windows) or shared (default for linux)
#!! Note: With GCC/Clang there seems to be no need to compile separately for SFML
#!! shared vs. static mode, so enough to just dispatch for DEBUG there!
#!! ?? Was this also true on native Linux, or was it only tested with W64devkit?!
#!! ?? And what about Clang?
#!! (I vaguely recall the separation only relevant for Windows DLLs, so likely
#!! both true (and then the VPAT/VTAG logic should obey!), but be sure + explicit!)
# - CRT_LINKMODE: static (default) or shared (MSVC CRT lib; i.e. CL /MT or /MD)
#!! ?? Again, how does this translate to GCC/Clang?
#-----------------------------------------------------------------------------
LIB_MODE      ?= static
DEBUG         ?= 0
SFML_DIR      ?= extern/sfml/$(TOOLCHAIN)
SFML_LINKMODE ?= $(if $(findstring linux,$(TOOLCHAIN)),shared,static)
#!! Only used for MSVC currently (to select -MD/-MT):
CRT_LINKMODE  ?= static

#-----------------------------------------------------------------------------
# Project layout...
#-----------------------------------------------------------------------------

NAME      := sfw
LIBNAME   := $(NAME)

# What to run for the `test` rule:
TEST       = $(TEST_DIR)/main
#TEST       = $(TEST_DIR)/smoke

LIBDIR      := lib/$(TOOLCHAIN)
SRCDIR      := src
#! Direct subdirs of $(SRCDIR) with explicitly supported different build semantics:
LIB_TAGDIR      := lib
TESTS_TAGDIR    := test
EXAMPLES_TAGDIR := examples

# Note the `=` below (and also that `tmp` is project-local):
OUTDIR     = tmp/build/$(VROOT)
OBJDIR     = $(OUTDIR)/o#!!... $(objext) <-- but this already has a dot prefix! :-/


TEST_DIR  := test
DEMO_DIR  := .

#!!TODO
#!! Make sure none of the ...DIR macros are empty (to avoid $(...DIR)/thing bugs)!
#!!TODO

#=============================================================================
# Workaround for #346
ifeq "$(CC)" "clang++"
GHA_OLD_CLANG_WORKAROUND := -stdlib=libc++
endif
#=============================================================================

CFLAGS_COMMON += -I$(SFML_DIR)/include $(GHA_OLD_CLANG_WORKAROUND)
#!! This may also be needed on Debian (not on Ubuntu) -- but just compiled it on my WSL Bullseye without it, so WTF?? --:
#!! LDFLAGS   := -pthread $(LDFLAGS)

# Switch various params. according to the host platform (i.e. the list
# of "auxilary" libs for static build, terminal control etc.)
#!!Incorrectly merged condition of build (host) platform and toolchain!
ifneq "$(BUILDENV_OS)" "windows"
# --- Not Windows (assuming "Linux-like & GCC-like"):
TERM_YELLOW   := \033[1;33m
TERM_GREEN    := \033[1;32m
TERM_NO_COLOR := \033[0m
AUXLIBS       := -lGL -lX11 -lXrandr -lXcursor -ludev
LDFLAGS       += -L./$(LIBDIR) -l$(LIBNAME) -L$(SFML_DIR)/lib
else
# --- Windows:
TERM_YELLOW   :=
TERM_GREEN    :=
TERM_NO_COLOR :=
ifneq "$(TOOLCHAIN)" "msvc"
AUXLIBS       := -lopengl32 -lwinmm -lgdi32
LDFLAGS       += -L./$(LIBDIR) -l$(LIBNAME) -L$(SFML_DIR)/lib
else
# --- MSVC:
AUXLIBS       := user32.lib kernel32.lib gdi32.lib winmm.lib advapi32.lib opengl32.lib
LDFLAGS       += /LIBPATH:./$(LIBDIR) $(LIBFILE_STATIC) /LIBPATH:$(SFML_DIR)/lib
endif
endif

#-----------------------------------------------------------------------------
# Prepare the artifact lists...
#-----------------------------------------------------------------------------

#!! Only *.cpp yet!...
#! Note: that cd is to omit the $(SRCDIR) root from the results for easier mapping to $(OBJDIR).
LIBSRC     := $(shell $(CD) "$(SRCDIR)"; $(FIND) "$(LIB_TAGDIR)" -name "*.cpp" -type f) # ... -not \( -path "$(EXCLUDE_SRCDIR)*" -type d -prune \) ...
#$(info LIBSRC = $(LIBSRC))
TESTSRC    := $(shell $(CD) "$(SRCDIR)"; $(FIND) "$(TESTS_TAGDIR)" -name "*.cpp" -type f)
#$(info TESTSRC = $(TESTSRC))
EXAMPLESRC := $(shell $(CD) "$(SRCDIR)"; $(FIND) "$(EXAMPLES_TAGDIR)" -name "*.cpp" -type f)
#$(info EXAMPLESRC = $(EXAMPLESRC))
#$(error x)

TEST_SRCDIR     := $(SRCDIR)/$(TESTS_TAGDIR)
EXAMPLES_SRCDIR := $(SRCDIR)/$(EXAMPLES_TAGDIR)

# Tag the executables with "-mingw", or nothing, respectively:
#!!OLD: TOOLCHAIN_TAG := $(if $(findstring linux,$(TOOLCHAIN)),-linux,-mingw)
TOOLCHAIN_TAG := -$(TOOLCHAIN)

# Mind the `=` vs. `:=` in the section below...

VROOT = $(TOOLCHAIN)/v.$(FILE_SUFFIX)#!! ...$(DIR_SUFFIX) <-- Use dots and/or subdirs instead of [-...[-...]]
#!!??VDIR = ...$(TOOLCHAIN_TAG)$(FILE_SUFFIX)
VTAG  = $(TOOLCHAIN_TAG)$(FILE_SUFFIX)
VTAG_NOTC = $(FILE_SUFFIX)

LIBOBJ         = $(LIBSRC:%.cpp=$(OBJDIR)/%$(objext))
#!! These aren't derived directly from ...SRC to allow decoupling the exes from single source files later:
_test_srcs     = $(shell $(FIND) $(TEST_SRCDIR) -name "*.cpp" -type f)
TEST_EXES      = $(_test_srcs:$(TEST_SRCDIR)/%.cpp=$(TEST_DIR)/%$(TOOLCHAIN_TAG)$(FILE_SUFFIX)$(exeext))
_example_srcs  = $(shell $(FIND) $(EXAMPLES_SRCDIR) -name "*.cpp" -type f)
EXAMPLE_EXES   = $(_example_srcs:$(EXAMPLES_SRCDIR)/%.cpp=$(DEMO_DIR)/%$(TOOLCHAIN_TAG)$(FILE_SUFFIX)$(exeext))

#!! This junk is needed to stop GNU make automatically deleting the object files! :-o
#!! (Except it didn't work; see comments at the "Main rules" section!...)
_test_objs     = $(_test_srcs:$(TEST_SRCDIR)/%.cpp=$(OBJDIR)/$(TESTS_TAGDIR)/%$(objext))
_example_objs  = $(_example_srcs:$(EXAMPLES_SRCDIR)/%.cpp=$(OBJDIR)/$(EXAMPLES_TAGDIR)/%$(objext))
          

#!!FILE_SUFFIX_DEBUG_1                      := -DBG
#!!DIR_SUFFIX_DEBUG_1                       := .DBG

FILE_SUFFIX_SFML_LINKMODE_shared_DEBUG_1 := -d
FILE_SUFFIX_SFML_LINKMODE_static_DEBUG_0 := -s
FILE_SUFFIX_SFML_LINKMODE_static_DEBUG_1 := -s-d

_sfml_lib_names := sfml-graphics sfml-window sfml-system
_sfml_lib_names_static := $(patsubst %,%-s,$(_sfml_lib_names))
_sfml_lib_names_debug := $(patsubst %,%-d,$(_sfml_lib_names))
_sfml_lib_names_static_debug := $(patsubst %,%-s-d,$(_sfml_lib_names))

CC_FLAGS_SFML_LINKMODE_static_DEBUG_0    := -DSFML_STATIC
CC_FLAGS_SFML_LINKMODE_static_DEBUG_1    := -DSFML_STATIC
CC_FLAGS_SFML_LINKMODE_shared_DEBUG_0    :=
CC_FLAGS_SFML_LINKMODE_shared_DEBUG_1    :=
LINK_FLAGS_SFML_LINKMODE_static_DEBUG_0  := $(patsubst %,-l%,$(_sfml_lib_names_static))  -lfreetype  $(AUXLIBS)
LINK_FLAGS_SFML_LINKMODE_static_DEBUG_1  := $(patsubst %,-l%,$(_sfml_lib_names_static_debug))  -lfreetype  $(AUXLIBS)
LINK_FLAGS_SFML_LINKMODE_shared_DEBUG_0  := $(patsubst %,-l%,$(_sfml_lib_names))  $(AUXLIBS)
LINK_FLAGS_SFML_LINKMODE_shared_DEBUG_1  := $(patsubst %,-l%,$(_sfml_lib_names_debug))  $(AUXLIBS)
ifeq "$(TOOLCHAIN)" "msvc"
# Add an extra -s to the static libs for my custom SFML build for MSVC/CRTstatic!...
ifeq "$(CRT_LINKMODE)" "static"
_s := -s
endif
LINK_FLAGS_SFML_LINKMODE_static_DEBUG_0  := $(patsubst %,%$(_s).lib,$(_sfml_lib_names_static))  freetype.lib $(AUXLIBS)
LINK_FLAGS_SFML_LINKMODE_static_DEBUG_1  := $(patsubst %,%$(_s)-d.lib,$(_sfml_lib_names_static))  freetype.lib $(AUXLIBS)
LINK_FLAGS_SFML_LINKMODE_shared_DEBUG_0  := $(patsubst %,%.lib,$(_sfml_lib_names))  $(AUXLIBS)
LINK_FLAGS_SFML_LINKMODE_shared_DEBUG_1  := $(patsubst %,%.lib,$(_sfml_lib_names_debug))  $(AUXLIBS)
endif

# If MSVC then add -MD for shared CRT, -MT for static CRT.
ifeq "$(TOOLCHAIN)" "msvc"
_crtmode_tag := $(if $(findstring static,$(CRT_LINKMODE)),-MT,-MD)
endif
FILE_SUFFIX   := $(_crtmode_tag)$(FILE_SUFFIX_SFML_LINKMODE_$(SFML_LINKMODE)_DEBUG_$(DEBUG))
CFLAGS_COMMON +=    $(CC_FLAGS_SFML_LINKMODE_$(SFML_LINKMODE)_DEBUG_$(DEBUG))
LDFLAGS       +=  $(LINK_FLAGS_SFML_LINKMODE_$(SFML_LINKMODE)_DEBUG_$(DEBUG))

LIBNAME  := $(LIBNAME)$(VTAG_NOTC)
LIBFILE_STATIC  := $(LIBDIR)/$(libname_prefix)$(LIBNAME)$(libext_static)
#!!LIBFILE_SHARED  := ...

#-----------------------------------------------------------------------------
define link_cmd =
	@echo "$(TERM_YELLOW)Linking $@$(TERM_NO_COLOR)"
	@$(LINKER) $< $(LDFLAGS)
endef
#!! Wow, couldn't just comment out the last line of link_cmd, so moved here:
#	@echo "$(TERM_GREEN)Done.$(TERM_NO_COLOR)"


#-----------------------------------------------------------------------------
# Main rules...
#-----------------------------------------------------------------------------
# Stop GNU make butchering incremental builds by default:
OBJS = $(LIBOBJ) $(_test_objs) $(_example_objs)
#$(info OBJS = $(OBJS))
#!! Didn't work:
#.PRECIOUS: $(OBJS)
#!! Also didn't work:
#.SECONDARY: $(OBJS)
# This finally did:
.SECONDARY:

$(info Build option DEBUG = $(DEBUG))
$(info Build option SFML_LINKMODE = $(SFML_LINKMODE))
$(info Build option CRT_LINKMODE = $(CRT_LINKMODE))
#$(info TEST_EXES = $(TEST_EXES))
#$(info EXAMPLE_EXES = $(EXAMPLE_EXES))

.PHONY: lib test_exes examples run_tests clean
all: lib test_exes examples #run_test

lib: $(LIBFILE_STATIC)
test_exes: $(TEST_EXES)
examples: $(EXAMPLE_EXES)

$(LIBFILE_STATIC): $(LIBOBJ)
	@$(ECHO) "$(TERM_YELLOW)Creating library$(TERM_NO_COLOR) $@"
	@$(MKDIR) $(LIBDIR)
	@$(LIBTOOL_STATIC) $(LIBTOOL_STATIC_FLAGS) $^
#	@ar crvf $@ $^

run_tests: test_exes
	@$(ECHO) "$(TERM_YELLOW)Running tests...$(TERM_NO_COLOR) $(TEST_EXE)"
	@$(TEST_EXE)

#!! OBSOLETE:
#clean:
#	@echo "$(TERM_YELLOW)Removing$(TERM_NO_COLOR) $(OBJDIR) & $(LIBFILE_STATIC)"
#	-@rm -r $(OBJDIR)
#	-@rm -r $(LIBFILE_STATIC)
#
#mrproper: clean
#	@echo "$(TERM_YELLOW)Removing$(TERM_NO_COLOR) $(DEMO_EXE)"
#	-@rm $(DEMO_EXE)
#	@echo "$(TERM_YELLOW)Removing$(TERM_NO_COLOR) $(TEST_EXE)"
#	-@rm $(TEST_EXE)
#	@echo "$(TERM_YELLOW)Removing$(TERM_NO_COLOR) $(SMOKE_TEST_EXE)"
#	-@rm $(SMOKE_TEST_EXE)

#-----------------------------------------------------------------------------
# Inference rules...
#-----------------------------------------------------------------------------

# Compile units for the lib
#! Done separately (see below), as they (may) need different options.
#! (E.g. no default lib vs. -MT[d]/-MD[d] etc.; see CLIBFLAGS vs. CEXEFLAGS.)
$(OBJDIR)/$(LIB_TAGDIR)/%$(objext): $(SRCDIR)/$(LIB_TAGDIR)/%.cpp
	@$(ECHO) "$(TERM_YELLOW)Compiling$(TERM_NO_COLOR) $<"
	@$(MKDIR) $(shell dirname $@)
	$(CXX) $(CXXFLAGS) $(CLIBFLAGS) -c $<

# Compile the rest (i.e. tests or examples):
#!! Could split this too by TESTS_TAGDIR/EXAMPLES_TAGDIR:
$(OBJDIR)/%$(objext): $(SRCDIR)/%.cpp
	@$(ECHO) "$(TERM_YELLOW)Compiling$(TERM_NO_COLOR) test/example: $<"
	@$(MKDIR) $(shell dirname $@)
	$(CXX) $(CXXFLAGS) $(CEXEFLAGS) -c $<

# Link each test source (via its .o) into a separate executable:
$(TEST_DIR)/%$(VTAG)$(exeext): $(OBJDIR)/$(TESTS_TAGDIR)/%$(objext) $(LIBFILE_STATIC)
	@$(MKDIR) $(shell dirname $@)
	$(link_cmd)

# Link each example source (via its .o) into a separate executable:
$(DEMO_DIR)/%$(VTAG)$(exeext): $(OBJDIR)/$(EXAMPLES_TAGDIR)/%$(objext) $(LIBFILE_STATIC)
	@$(MKDIR) $(shell dirname $@)
	$(link_cmd)


#-----------------------------------------------------------------------------
# Header dependencies...
#-----------------------------------------------------------------------------

-include $(OBJS:%$(objext)=%.d)

# Copied from the DarkPlaces Quake clone makefile, as a hommage:
# hack to deal with no-longer-needed .h files
%.h:
	@echo
	@echo "NOTE: file $@ mentioned in dependencies missing, continuing..."
#	@echo "HINT: consider 'make clean'"
	@echo


ifeq "1" "0"
$(info VROOT: $(VROOT))
$(info VTAG: $(VTAG))
$(info OBJDIR: $(OBJDIR))
$(info LIBNAME: $(LIBNAME))
$(error ABORT)
endif
