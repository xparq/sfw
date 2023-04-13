#-----------------------------------------------------------------------------
# GNU Makefile for GCC/CLANG -- on both "native GNU" (i.e. Linux etc.) and
# Windows (i.e. MinGW, w64devkit or compatible)
#
# Dependencies: 
# - SFML: should be preinstalled locally (e.g. via tooling/sfml-setup, which
#   currently only works on Linux)
# - OpenGL & other platform deps (see AUXLIBS): should be provided by the host
#   system (e.g. the GHA job installs them before launching the build!)
# - freetype: provided by the host for shared builds, or bundled with SFML
#   for static
# NOTE: Make sure SFML_DIR points to the correct SFML package, and that the
#       SFML dir suffix (if any) matches $(TOOLCHAIN)!
#-----------------------------------------------------------------------------

# "Detect" default GCC/CLANG flavor ("mingw" on Windows, "linux" otherwise;
# rather hamfisted, but OK for now) -- use `make TOOLCHAIN=...` to override!):
ifeq "$(shell echo ${WINDIR})" ""
TOOLCHAIN ?= linux
else
TOOLCHAIN ?= mingw
endif

# Use `make SFML_DIR=...` to override:
SFML_DIR  ?= extern/sfml.$(TOOLCHAIN)

# Supported build alternatives: debug/release, shared/static SFML:
#   DEBUG must be 0 or 1
#   SFML_LINKMODE must be shared or static
DEBUG     ?= 0
SFML_LINKMODE ?= $(if $(findstring linux,$(TOOLCHAIN)),shared,static)

#-----------------------------------------------------------------------------
# Project layout

NAME      := sfw
LIBNAME   := $(NAME)

DEMO      := $(NAME)-demo
TEST      := $(NAME)-test
SMOKE_TEST:= test/smoke

LIBDIR    := lib/$(TOOLCHAIN)
SRCDIR    := src
SRC       := $(shell cd "$(SRCDIR)"; find . -name "*.cpp" -type f)
OUTDIR    := tmp/build/$(TOOLCHAIN)
OBJDIR    := $(OUTDIR)
DEMO_OBJ       := examples/demo
TEST_OBJ       := test/main
SMOKE_TEST_OBJ := test/smoke
          
CC        := g++
CFLAGS    := -I$(SFML_DIR)/include -I./include -std=c++20 -pedantic -Wall -Wextra -Wshadow -Wwrite-strings -O2
LDFLAGS   := $(LDFLAGS) -L$(SFML_DIR)/lib

# Switch various params. according to the host platform (i.e. the list
# of "auxilary" libs for static build, terminal control etc.)
ifeq "$(shell echo ${WINDIR})" ""
# --- Not Windows (assuming "something Linux-like"):
AUXLIBS       := -lGL -lX11 -lXrandr -lXcursor -ludev
TERM_YELLOW   := \033[1;33m
TERM_GREEN    := \033[1;32m
TERM_NO_COLOR := \033[0m
else
# --- Windows:
AUXLIBS       := -lopengl32 -lwinmm -lgdi32
EXE_SUFFIX    :=.exe
endif

#-----------------------------------------------------------------------------
# Tag the executables with "-mingw", or nothing, respectively:
TOOLCHAIN_TAG := $(if $(findstring linux,$(TOOLCHAIN)),,-mingw)

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

DEMO_EXE := $(DEMO)$(TOOLCHAIN_TAG)$(FILE_SUFFIX)$(EXE_SUFFIX)
TEST_EXE := $(TEST)$(TOOLCHAIN_TAG)$(FILE_SUFFIX)$(EXE_SUFFIX)
SMOKE_TEST_EXE := $(SMOKE_TEST)$(EXE_SUFFIX)
# Under GCC the same lib (i.e. compilation mode) seems to work just fine 
# for linking with both the static and dynamic SFML libs, so enough to just
# dispatch for debug mode:
OBJDIR   := $(OUTDIR)$(DIR_SUFFIX_DEBUG_$(DEBUG))
OBJ      := $(SRC:%.cpp=$(OBJDIR)/%.o)
LIBNAME  := $(LIBNAME)$(FILE_SUFFIX_DEBUG_$(DEBUG))
LIBFILE  := $(LIBDIR)/lib$(LIBNAME).a

#-----------------------------------------------------------------------------
define link-exe =
	@echo "$(TERM_YELLOW)Linking $@$(TERM_NO_COLOR)"
	@$(CC) $< $(CFLAGS) -L./$(LIBDIR) -l$(LIBNAME) $(LDFLAGS) -o $@
	@echo "$(TERM_GREEN)Done.$(TERM_NO_COLOR)"
endef

#-----------------------------------------------------------------------------
$(info Build option DEBUG = $(DEBUG))
$(info Build option SFML_LINKMODE = $(SFML_LINKMODE))

all: $(SMOKE_TEST_EXE) $(TEST_EXE) $(DEMO_EXE)

$(DEMO_EXE): $(OBJDIR)/$(DEMO_OBJ).o $(LIBFILE)
	$(link-exe)

$(TEST_EXE): $(OBJDIR)/$(TEST_OBJ).o $(LIBFILE)
	$(link-exe)

$(SMOKE_TEST_EXE): $(OBJDIR)/$(SMOKE_TEST_OBJ).o $(LIBFILE)
	$(link-exe)

$(LIBFILE): $(OBJ)
	@mkdir -p $(LIBDIR)
	@echo "$(TERM_YELLOW)Creating library$(TERM_NO_COLOR) $@"
	@ar crvf $@ $(OBJ)

# Obj. files for the library, test, demo, examples etc.
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "$(TERM_YELLOW)Compiling$(TERM_NO_COLOR) $<"
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_EXE)
	@echo "$(TERM_YELLOW)Running tests...$(TERM_NO_COLOR) $(TEST_EXE)"
	@$(TEST_EXE)

clean:
	@echo "$(TERM_YELLOW)Removing$(TERM_NO_COLOR) $(OUTDIR) & $(LIBDIR)"
	-@rm -r $(LIBDIR)
	-@rm -r $(OUTDIR)

mrproper: clean
	@echo "$(TERM_YELLOW)Removing$(TERM_NO_COLOR) $(DEMO_EXE)"
	-@rm $(DEMO_EXE)
	@echo "$(TERM_YELLOW)Removing$(TERM_NO_COLOR) $(TEST_EXE)"
	-@rm $(TEST_EXE)
	@echo "$(TERM_YELLOW)Removing$(TERM_NO_COLOR) $(SMOKE_TEST_EXE)"
	-@rm $(SMOKE_TEST_EXE)
