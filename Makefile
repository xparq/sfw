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

# "Detect" default GCC/CLANG flavor ("mingw" on Windows, "linux" otherwise;
# rather hamfisted, but OK for now) -- use `make TOOLCHAIN=...` to override!):
ifeq "$(shell echo ${WINDIR})" ""
TOOLCHAIN ?= linux
else
TOOLCHAIN ?= mingw
endif

#-----------------------------------------------------------------------------
# Build options...
#
# - DEBUG must be 0 or 1 (default: 0)
# - CC: g++ or clang++ (default: g++)
# - SFML_LINKMODE must be static or shared (default: shared for linux, else static)
# - SFML_DIR must point to a pre-built SFML package dir matching $(TOOLCHAIN)
#-----------------------------------------------------------------------------
DEBUG         ?= 0
SFML_LINKMODE ?= $(if $(findstring linux,$(TOOLCHAIN)),shared,static)
SFML_DIR      ?= extern/sfml/$(TOOLCHAIN)

#-----------------------------------------------------------------------------
# Project layout...
#-----------------------------------------------------------------------------

NAME      := sfw
LIBNAME   := $(NAME)

DEMO       = $(NAME)-demo

# What to run for the `test` rule:
TEST       = $(TEST_DIR)/main
#TEST       = $(TEST_DIR)/smoke

LIBDIR      := lib/$(TOOLCHAIN)
SRCDIR      := src
#! Direct subdirs of $(SRCDIR) with explicitly supported different build semantics:
LIB_TAGDIR     := lib
TESTS_TAGDIR    := test
EXAMPLES_TAGDIR := examples

OUTDIR    := tmp/build/$(TOOLCHAIN)
OBJDIR    := $(OUTDIR)
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

CC        := g++
CFLAGS    := -I$(SFML_DIR)/include -I./include -std=c++23 $(GHA_OLD_CLANG_WORKAROUND) -pedantic -Wall -Wextra -Wshadow -Wwrite-strings -O2
LDFLAGS   := $(LDFLAGS) -L$(SFML_DIR)/lib
#!! This may also be needed on Debian (not on Ubuntu) -- but just compiled it on my WSL Bullseye without it, so WTF?? --:
#!! LDFLAGS   := -pthread $(LDFLAGS)

# Switch various params. according to the host platform (i.e. the list
# of "auxilary" libs for static build, terminal control etc.)
#!!Incorrectly merged condition of build (host) platform and toolchain!
ifeq "$(shell echo ${WINDIR})" ""
# --- Not Windows (assuming "something Linux-like"):
AUXLIBS       := -lGL -lX11 -lXrandr -lXcursor -ludev
EXE_SUFFIX    :=
#!!OBJ_SUFFIX    := .o
TERM_YELLOW   := \033[1;33m
TERM_GREEN    := \033[1;32m
TERM_NO_COLOR := \033[0m
else
# --- Windows:
AUXLIBS       := -lopengl32 -lwinmm -lgdi32
EXE_SUFFIX    := .exe
#!!OBJ_SUFFIX    := .o
endif
#!! See comment above; also, only GCC style is supported yet! (See e.g. OON for a less caveman-like example!)
OBJ_SUFFIX    := .o

#-----------------------------------------------------------------------------
# Prepare the artifact lists...
#-----------------------------------------------------------------------------

#!! Only *.cpp yet!...
#! Note: that cd is to omit the $(SRCDIR) root from the results for easier mapping to $(OBJDIR).
LIBSRC     := $(shell cd "$(SRCDIR)"; find "$(LIB_TAGDIR)" -name "*.cpp" -type f) # ... -not \( -path "$(EXCLUDE_SRCDIR)*" -type d -prune \) ...
#$(info LIBSRC = $(LIBSRC))
TESTSRC    := $(shell cd "$(SRCDIR)"; find "$(TESTS_TAGDIR)" -name "*.cpp" -type f)
#$(info TESTSRC = $(TESTSRC))
EXAMPLESRC := $(shell cd "$(SRCDIR)"; find "$(EXAMPLES_TAGDIR)" -name "*.cpp" -type f)
#$(info EXAMPLESRC = $(EXAMPLESRC))
#$(error x)

TEST_SRCDIR     := $(SRCDIR)/$(TESTS_TAGDIR)
EXAMPLES_SRCDIR := $(SRCDIR)/$(EXAMPLES_TAGDIR)

LIBOBJ         = $(LIBSRC:%.cpp=$(OBJDIR)/%$(TOOLCHAIN_TAG)$(FILE_SUFFIX)$(OBJ_SUFFIX))
#!! These aren't derived directly from ...SRC to allow decoupling the exes from single source files later:
_test_srcs     = $(shell find $(TEST_SRCDIR) -name "*.cpp" -type f)
TEST_EXES      = $(_test_srcs:$(TEST_SRCDIR)/%.cpp=$(TEST_DIR)/%$(TOOLCHAIN_TAG)$(FILE_SUFFIX)$(EXE_SUFFIX))
_example_srcs  = $(shell    find $(EXAMPLES_SRCDIR) -name "*.cpp" -type f)
EXAMPLE_EXES   = $(_example_srcs:$(EXAMPLES_SRCDIR)/%.cpp=$(DEMO_DIR)/%$(TOOLCHAIN_TAG)$(FILE_SUFFIX)$(EXE_SUFFIX))

#!! This junk is needed to stop GNU make automatically deleting the object files! :-o
#!! (Except it didn't work; see comments at the "Main rules" section!...)
#_test_objs     = $(_test_srcs:$(TEST_SRCDIR)/%.cpp=$(OBJDIR)/$(TESTS_TAGDIR)/%$(TOOLCHAIN_TAG)$(FILE_SUFFIX)$(OBJ_SUFFIX)))
#_example_objs  = $(_example_srcs:$(EXAMPLES_SRCDIR)/%.cpp=$(OBJDIR)/$(EXAMPLES_TAGDIR)/%$(TOOLCHAIN_TAG)$(FILE_SUFFIX)$(OBJ_SUFFIX)))
          

# Tag the executables with "-mingw", or nothing, respectively:
TOOLCHAIN_TAG := $(if $(findstring linux,$(TOOLCHAIN)),-linux,-mingw)

FILE_SUFFIX_DEBUG_1                      := -d
DIR_SUFFIX_DEBUG_1                       := .d
FILE_SUFFIX_SFML_LINKMODE_shared_DEBUG_1 := -d
FILE_SUFFIX_SFML_LINKMODE_static_DEBUG_0 := -s
FILE_SUFFIX_SFML_LINKMODE_static_DEBUG_1 := -s-d
CC_FLAGS_SFML_LINKMODE_static_DEBUG_0    := -DNDEBUG -DSFML_STATIC
CC_FLAGS_SFML_LINKMODE_static_DEBUG_1    := -DDEBUG -DSFML_STATIC
CC_FLAGS_SFML_LINKMODE_shared_DEBUG_0    := -DNDEBUG
CC_FLAGS_SFML_LINKMODE_shared_DEBUG_1    := -DDEBUG
LINK_FLAGS_SFML_LINKMODE_static_DEBUG_0  := -static -lsfml-graphics-s   -lsfml-window-s   -lsfml-system-s   -lfreetype $(AUXLIBS)
LINK_FLAGS_SFML_LINKMODE_static_DEBUG_1  := -static -lsfml-graphics-s-d -lsfml-window-s-d -lsfml-system-s-d -lfreetype $(AUXLIBS)
LINK_FLAGS_SFML_LINKMODE_shared_DEBUG_0  := -lsfml-graphics -lsfml-window -lsfml-system       $(AUXLIBS)
LINK_FLAGS_SFML_LINKMODE_shared_DEBUG_1  := -lsfml-graphics-d -lsfml-window-d -lsfml-system-d $(AUXLIBS)

FILE_SUFFIX :=           $(FILE_SUFFIX_SFML_LINKMODE_$(SFML_LINKMODE)_DEBUG_$(DEBUG))
CFLAGS      :=    $(CFLAGS) $(CC_FLAGS_SFML_LINKMODE_$(SFML_LINKMODE)_DEBUG_$(DEBUG))
LDFLAGS     := $(LDFLAGS) $(LINK_FLAGS_SFML_LINKMODE_$(SFML_LINKMODE)_DEBUG_$(DEBUG))

# Under GCC the same lib (i.e. compilation mode) seems to work just fine 
# for linking with both the static and dynamic SFML libs, so enough to just
# dispatch for debug mode:
OBJDIR   := $(OUTDIR)/o$(DIR_SUFFIX_DEBUG_$(DEBUG))
LIBNAME  := $(LIBNAME)$(FILE_SUFFIX_DEBUG_$(DEBUG))
LIBFILE  := $(LIBDIR)/lib$(LIBNAME).a

#-----------------------------------------------------------------------------
define link_cmd =
	@echo "$(TERM_YELLOW)Linking $@$(TERM_NO_COLOR)"
	$(CC) $< $(CFLAGS) -L./$(LIBDIR) -l$(LIBNAME) $(LDFLAGS) -o $@
	@echo "$(TERM_GREEN)Done.$(TERM_NO_COLOR)"
endef


#-----------------------------------------------------------------------------
# Main rules...
#-----------------------------------------------------------------------------
# Stop GNU make butchering incremental builds by default:
#OBJS = $(LIBOBJ) $(_test_objs) $(_example_objs)
#$(info OBJS = $(OBJS))
#!! Didn't work:
#.PRECIOUS: $(OBJS)
#!! Also didn't work:
#.SECONDARY: $(OBJS)
# This finally did:
.SECONDARY:

$(info Build option DEBUG = $(DEBUG))
$(info Build option SFML_LINKMODE = $(SFML_LINKMODE))
#$(info TEST_EXES = $(TEST_EXES))
#$(info EXAMPLE_EXES = $(EXAMPLE_EXES))

.PHONY: lib test_exes examples run_tests clean
all: lib test_exes examples #run_test

lib: $(LIBFILE)
test_exes: $(TEST_EXES)
examples: $(EXAMPLE_EXES)

$(LIBFILE): $(LIBOBJ)
	@echo "$(TERM_YELLOW)Creating library$(TERM_NO_COLOR) $@"
	@mkdir -p $(LIBDIR)
	@ar crvf $@ $^

run_tests: test_exes
	@echo "$(TERM_YELLOW)Running tests...$(TERM_NO_COLOR) $(TEST_EXE)"
	@$(TEST_EXE)

#!! OBSOLETE:
#clean:
#	@echo "$(TERM_YELLOW)Removing$(TERM_NO_COLOR) $(OBJDIR) & $(LIBFILE)"
#	-@rm -r $(OBJDIR)
#	-@rm -r $(LIBFILE)
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

# Common rule to compile any of the obj files (mirroring the source tree in
# $(OBJDIR), and tagging the targets according to the build mode):
$(OBJDIR)/%$(TOOLCHAIN_TAG)$(FILE_SUFFIX).o: $(SRCDIR)/%.cpp
	@echo "$(TERM_YELLOW)Compiling$(TERM_NO_COLOR) $<"
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# Link each test source (via its .o) into a separate executable:
$(TEST_DIR)/%$(EXE_SUFFIX): $(OBJDIR)/$(TESTS_TAGDIR)/%.o $(LIBFILE)
	@mkdir -p $(shell dirname $@)
	$(link_cmd)

# Link each example source (via its .o) into a separate executable:
$(DEMO_DIR)/%$(EXE_SUFFIX): $(OBJDIR)/$(EXAMPLES_TAGDIR)/%.o $(LIBFILE)
	@mkdir -p $(shell dirname $@)
	$(link_cmd)
