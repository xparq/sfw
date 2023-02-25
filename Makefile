LIBNAME := sfw
DEMO    := $(LIBNAME)-demo
LIBDIR  := lib
LIBFILE := $(LIBDIR)/lib$(LIBNAME).a
SRCDIR  := src
SRC     := $(shell find $(SRCDIR) -name "*.cpp" -type f)
OUTDIR  := tmp/build
OBJDIR  := $(OUTDIR)
OBJ     := $(SRC:%.cpp=$(OBJDIR)/%.o)
ifndef SFML_DIR
SFML_DIR := extern/sfml
endif
CC      := g++
CFLAGS  := -I$(SFML_DIR)/include -I./include -std=c++20 -pedantic -Wall -Wextra -Wshadow -Wwrite-strings -O2
LDFLAGS := -L$(SFML_DIR)/lib -lsfml-graphics -lsfml-window -lsfml-system -lGL

# Demo
$(DEMO): src/demo.cpp $(LIBFILE)
	@echo "\033[1;33mlinking executable\033[0m $@"
	@$(CC) $< $(CFLAGS) -L./$(LIBDIR) -l$(LIBNAME) $(LDFLAGS) -o $@
	@echo "\033[1;32mDone!\033[0m"

# Static library
$(LIBFILE): $(OBJ)
	@mkdir -p $(LIBDIR)
	@echo "\033[1;33mlinking library\033[0m $@"
	@ar crvf $@ $(OBJ)

# Library objects
$(OBJDIR)/%.o: %.cpp
	@echo "\033[1;33mcompiling\033[0m $<"
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "\033[1;33mremoving\033[0m $(OUTDIR)"
	-@rm -r $(LIBDIR)
	-@rm -r $(OUTDIR)

mrproper: clean
	@echo "\033[1;33mremoving\033[0m $(DEMO)"
	-@rm $(DEMO)

all: mrproper $(DEMO)
