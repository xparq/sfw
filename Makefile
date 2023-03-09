LIBNAME := sfw
DEMO    := $(LIBNAME)-demo
TEST    := $(LIBNAME)-test
LIBDIR  := lib
LIBFILE := $(LIBDIR)/lib$(LIBNAME).a
SRCDIR  := src
SRC     := $(shell cd "$(SRCDIR)"; find . -name "*.cpp" -type f)
OUTDIR  := tmp/build
OBJDIR  := $(OUTDIR)
OBJ     := $(SRC:%.cpp=$(OBJDIR)/%.o)
DEMO_OBJ:= demo
TEST_OBJ:= test/main
ifndef SFML_DIR
SFML_DIR:= extern/sfml
endif
CC      := g++
CFLAGS  := -I$(SFML_DIR)/include -I./include -std=c++20 -pedantic -Wall -Wextra -Wshadow -Wwrite-strings -O2
LDFLAGS := -L$(SFML_DIR)/lib -lGL

#-----------------------------------------------------------------------------
# Debug / Release adjustments (Use `make DEBUG=1` for a debug build!):
ifndef DEBUG
DEBUG=0
endif
CC_FLAGS_DEBUG_0=-DNDEBUG
CC_FLAGS_DEBUG_1=-DDEBUG
LINK_FLAGS_DEBUG_0=-lsfml-graphics -lsfml-window -lsfml-system
LINK_FLAGS_DEBUG_1=-lsfml-graphics-d -lsfml-window-d -lsfml-system-d
CFLAGS  := $(CFLAGS) $(CC_FLAGS_DEBUG_$(DEBUG))
LDFLAGS := $(LDFLAGS) $(LINK_FLAGS_DEBUG_$(DEBUG))

#-----------------------------------------------------------------------------
# Demo
$(DEMO): $(OBJDIR)/$(DEMO_OBJ).o $(LIBFILE)
	@echo "\033[1;33mlinking the demo\033[0m $@"
	@$(CC) $< $(CFLAGS) -L./$(LIBDIR) -l$(LIBNAME) $(LDFLAGS) -o $@
	@echo "\033[1;32mDone!\033[0m"

# Test
$(TEST): $(OBJDIR)/$(TEST_OBJ).o $(LIBFILE)
	@echo "\033[1;33mlinking the test executable\033[0m $@"
	@$(CC) $< $(CFLAGS) -L./$(LIBDIR) -l$(LIBNAME) $(LDFLAGS) -o $@
	@echo "\033[1;32mDone!\033[0m"

# Static library
$(LIBFILE): $(OBJ)
	@mkdir -p $(LIBDIR)
	@echo "\033[1;33mcreating library\033[0m $@"
	@ar crvf $@ $(OBJ)

# Library, test, demo etc. object modules
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "\033[1;33mcompiling\033[0m $<"
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST)
	@echo "\033[1;33mRunning tests...\033[0m $(TEST)"
	@$(TEST)

clean:
	@echo "\033[1;33mremoving\033[0m $(OUTDIR)"
	-@rm -r $(LIBDIR)
	-@rm -r $(OUTDIR)

mrproper: clean
	@echo "\033[1;33mremoving\033[0m $(DEMO)"
	-@rm $(DEMO)
	@echo "\033[1;33mremoving\033[0m $(TEST)"
	-@rm $(TEST)

all: mrproper $(TEST) $(DEMO)
