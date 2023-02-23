TARGET  := sfml-widgets-demo
SRCDIR  := src
SRC     := $(shell find $(SRCDIR) -name "*.cpp" -type f)
OBJDIR  := out
OBJ     := $(SRC:%.cpp=$(OBJDIR)/%.o)
ifndef SFML_DIR
SFML_DIR := ext/sfml
endif
CC      := g++
CFLAGS  := -I$(SFML_DIR)/include -I$(SRCDIR) -std=c++20 -pedantic -Wall -Wextra -Wshadow -Wwrite-strings -O2
LDFLAGS := -L$(SFML_DIR)/lib -lsfml-graphics -lsfml-window -lsfml-system -lGL

# Demo
$(TARGET): src/demo.cpp lib/libsfml-widgets.a
	@echo "\033[1;33mlinking exec\033[0m $@"
	@$(CC) $< $(CFLAGS) -L./lib -lsfml-widgets $(LDFLAGS) -o $@
	@echo "\033[1;32mDone!\033[0m"

# Static library
lib/libsfml-widgets.a: $(OBJ)
	@mkdir -p lib
	@echo "\033[1;33mlinking library\033[0m $@"
	@ar crvf $@ $(OBJ)

# Library objects
$(OBJDIR)/%.o: %.cpp
	@echo "\033[1;33mcompiling\033[0m $<"
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "\033[1;33mremoving\033[0m $(OBJDIR)"
	-@rm -r lib
	-@rm -r $(OBJDIR)

mrproper: clean
	@echo "\033[1;33mremoving\033[0m $(TARGET)"
	-@rm $(TARGET)

all: mrproper $(TARGET)
