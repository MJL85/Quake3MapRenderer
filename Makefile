# Minimal Makefile
#
# By: Michael Laforest < thepara (--AT--) g m a i l [--DOT--] com >
#
# $Header$

PROJECT_NAME = typeball

CC = gcc
CCX = g++

CFLAGS = -Wall -pipe
FLAGS = $(CFLAGS)
DFLAGS = $(CFLAGS) -g
LDFLAGS = -lGL -lGLU -lSDL -lSDL_image

#
# Target binaries (always created as BIN)
#
BIN = $(PROJECT_NAME)
DBIN = $(PROJECT_NAME)-debug

INCLUDES = -I. -Iinclude/

#
# Directories with source files.
#
SRC_DIRS = src src/engine src/render src/math

#
# Find all source files in each directory.
#
C_SRC_FILES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
CPP_SRC_FILES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))

SRC_FILES = $(C_SRC_FILES) $(CPP_SRC_FILES)

#
# Generate a list of object files
#
C_OBJS = $(C_SRC_FILES:%.c=%.o)
CPP_OBJS = $(CPP_SRC_FILES:%.cpp=%.o)

OBJS = $(C_OBJS) $(CPP_OBJS)
DOBJS = $(OBJS:%.o=%.debug_o)

#
# Need this for the 'clean' target.
#
EXISTING_RELEASE_OBJS = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.o))
EXISTING_DEBUG_OBJS = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.debug_o))
EXISTING_OBJS = $(EXISTING_RELEASE_OBJS) $(EXISTING_DEBUG_OBJS) $(BIN)


###############################
#
# Build targets.
#
###############################

Release:
	@echo
	@echo ------
	@echo - Building $(PROJECT_NAME) ...
	@echo ------
	@make $(BIN)

	@echo
	@echo --
	@echo -- Build complete.
	@echo --
	@echo

Debug: $(DBIN)

release: Release
debug: Debug

#
# If they exist, remove the files:
#   *.o
#
clean:
	@if [ ! -z "$(EXISTING_OBJS)" ]; \
	then \
		rm $(EXISTING_OBJS) &> /dev/null ; \
		echo "Cleaned files."; \
	else \
		echo "No files to clean."; \
	fi

$(BIN): $(OBJS)
	$(CCX) $(FLAGS) $(LDFLAGS) $(OBJS) -o $(BIN)

$(DBIN): $(DOBJS)
	$(CCX) $(DFLAGS) $(LDFLAGS) $(DOBJS) -o $(BIN)

%.o: %.c
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(CCX) $(FLAGS) $(INCLUDES) -c $< -o $@

%.debug_o: %.c
	$(CC) $(DFLAGS) $(INCLUDES) -c $< -o $@

%.debug_o: %.cpp
	$(CCX) $(DFLAGS) $(INCLUDES) -c $< -o $@
