# Comprehensive Makefile
# This will automatically compile any .c files in the current directory.

CC = gcc
CFLAGS = --std=gnu99
SRCEXT = c

BINDIR = .
exe_file = $(BINDIR)/$(shell basename "${PWD}")

# Handle debug case
DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CFLAGS += -g -Wall
else
	CFLAGS += -DNDEBUG -O3
endif

SRCDIR = .
BUILDDIR = build
SOURCES = $(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
DEP = $(OBJECTS:.o=.d)

all: $(exe_file)

$(exe_file): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) -o $(exe_file) $^ $(LIB) $(LDFLAGS)

$(BUILDDIR)/%.d: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@$(CC) $(INC) $< -MM -MT $(@:.d=.o) >$@

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf $(BUILDDIR) $(exe_file)

-include $(DEP)

