# Copyright (c) 2016 Gustavo Pantuza Coelho Pinto
# Copyright (c) 2023 Lai-YT
#
# Can be distributed under the MIT license.
# The full text of the license can be found in
# https://opensource.org/license/mit/.

# Includes the project configurations
include project.conf

#
# Validating project variables defined in project.conf
#
ifndef PROJECT_NAME
$(error Missing PROJECT_NAME. Put variables at project.conf file)
endif
ifndef BINARY
$(error Missing BINARY. Put variables at project.conf file)
endif
ifndef PROJECT_PATH
$(error Missing PROJECT_PATH. Put variables at project.conf file)
endif


# Gets the Operating system name
OS := $(shell uname -s)

# Default shell
SHELL := bash

# Color prefix for Linux distributions
COLOR_PREFIX := e

ifeq ($(OS),Darwin)
	COLOR_PREFIX := 033
endif

# Color definition for print purpose
YELLOW=\$(COLOR_PREFIX)[0;33m
BLUE=\$(COLOR_PREFIX)[1;34m
END_COLOR=\$(COLOR_PREFIX)[0m


# Source code directory structure
BINDIR := bin
SRCDIR := src
LOGDIR := log
LIBDIR := lib
TESTDIR := test


# Source code file extension
SRCEXT := c


# Defines the C Compiler
CC := gcc


# Defines the language standards for GCC
STD := -std=gnu99 # See man gcc for more options

# Protection for stack-smashing attack
STACK := -fstack-protector-all -Wstack-protector

# Specifies to GCC the required warnings
WARNS := -Wall -Wextra -pedantic # -pedantic warns on language standards

# Flags for compiling
CFLAGS := $(STD) $(STACK) $(WARNS)

# Flags differ between debug and release build
DEBUG := -O0 -g3 -DDEBUG=1
RELEASE := -O3

# Flags for customizable format check, e.g., FMTFLAGS='--dry-run --Werror'
FMTFLAGS := -i

# Dependency libraries
LIBS := -lm

# Test libraries
TEST_LIBS := -l cmocka


# Tests binary file
TEST_BINARY := $(BINARY)_test_runner


# %.o file names
NAMES := $(notdir $(basename $(wildcard $(SRCDIR)/*.$(SRCEXT))))
OBJECTS := $(patsubst %,$(LIBDIR)/%.o,$(NAMES))


#
# COMPILATION RULES
#

default: debug

# Help message
help:
	@echo -e "$(BLUE)$(PROJECT_NAME)$(END_COLOR)"
	@echo
	@echo "Target rules:"
	@echo "    debug    - Compiles and generates binary file with"
	@echo "               debug messages and less optimizations"
	@echo "    release  - Compiles and generates optimized binary file"
	@echo "    tests    - Compiles with cmocka and runs test binary file"
	@echo "    valgrind - Runs test binary file using valgrind tool"
	@echo "    fmt      - Formats the source and test files"
	@echo "    tidy     - Checks naming conventions and bug-proneness"
	@echo "    clean    - Cleans the project by removing binaries"
	@echo "    help     - Prints a help message with target rules"

# Creates the directories needed for compilation
build_dir:
	@mkdir -p $(LIBDIR) $(BINDIR)

# Rule for link and generate the binary file
debug: CFLAGS += $(DEBUG)
release: CFLAGS += $(RELEASE)
debug release: build_dir $(OBJECTS)
	@echo -en "$(YELLOW)LD $(END_COLOR)";
	$(CC) -o $(BINDIR)/$(BINARY) $(filter-out $<,$^) $(CFLAGS) $(LIBS)
	@echo -e "$(YELLOW)End $@ build.$(END_COLOR)"
	@echo -en "\n--\nBinary file placed at" \
			  "$(YELLOW)$(BINDIR)/$(BINARY)$(END_COLOR)\n";


# Rule for object binaries compilation
$(LIBDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo -en "$(YELLOW)CC $(END_COLOR)";
	$(CC) -c $^ -o $@ $(CFLAGS) $(LIBS)


# Rule for run valgrind tool
valgrind:
	@mkdir -p $(LOGDIR)
	@valgrind \
		--track-origins=yes \
		--leak-check=full \
		--leak-resolution=high \
		--error-exitcode=1 \
		--log-file=$(LOGDIR)/$@.log \
		$(BINDIR)/$(TEST_BINARY); \
	EXIT_CODE=$$?; \
	echo -e "\nCheck the log file: $(LOGDIR)/$@.log"; \
	exit $${EXIT_CODE}


# Rule for formatting the source code with clang-format
fmt:
	@clang-format -style=file \
		$(FMTFLAGS) \
		{$(SRCDIR),$(TESTDIR)}/*.{h,c}

# Rule for enforcing naming conventions and checking proneness to bugs with clang-tidy
tidy:
	@clang-tidy --quiet \
		{$(SRCDIR),$(TESTDIR)}/*.{h,c} \
		-- $(CFLAGS)

# Compile tests and run the test binary
tests: debug
	@echo -en "$(YELLOW)CC $(END_COLOR)";
	$(CC) $(TESTDIR)/main.c -o $(BINDIR)/$(TEST_BINARY) $(shell find $(LIBDIR) -name *.o ! -name main.o) $(DEBUG) $(CFLAGS) $(LIBS) $(TEST_LIBS)
	@which ldconfig && ldconfig -C /tmp/ld.so.cache || true # caching the library linking
	@echo -en "$(YELLOW) Running tests: $(END_COLOR)";
	./$(BINDIR)/$(TEST_BINARY)


# Rule for cleaning the project
clean:
	@rm -rvf $(BINDIR)/* $(LIBDIR)/* $(LOGDIR)/*;
