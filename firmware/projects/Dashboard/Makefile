#
# Makefile
# WARNING: relies on invocation setting current working directory to Makefile location
# This is done in .vscode/task.json
#
MAKEFLAGS           := -j $(shell nproc)
SRC_EXT             := c cpp        # Add cpp extension
OBJ_EXT             := o
CC                  := gcc
CXX                 := g++          # Add C++ compiler

SRC_DIR             := ./
WORKING_DIR         := ./build
BUILD_DIR           := $(WORKING_DIR)/obj
BIN_DIR             := $(WORKING_DIR)/bin
UI_DIR              := ui

WARNINGS            := -Wall -Wextra \
                        -Wshadow -Wundef -Wmaybe-uninitialized -Wno-discarded-qualifiers \
                        -Wno-unused-function -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized \
                        -Wno-unused-parameter -Wno-missing-field-initializers -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default  \
                        -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wdouble-promotion -Wclobbered -Wdeprecated  \
                        -Wempty-body -Wshift-negative-value -Wstack-usage=2048 \
                        -Wtype-limits -Wsizeof-pointer-memaccess -Wpointer-arith

CFLAGS              := -O0 -g $(WARNINGS)
CXXFLAGS            := $(CFLAGS)    # Add C++ flags

# Add simulator define to allow modification of source
DEFINES             := -D SIMULATOR=1 -D LV_BUILD_TEST=0

# Include simulator inc folder first so lv_conf.h from custom UI can be used instead
INC                 := -I./ui/simulator/inc/ -I./ -I./lvgl/
LDFLAGS             := -lSDL2 -lm
BIN                 := $(BIN_DIR)/demo

COMPILE_C           = $(CC) $(CFLAGS) $(INC) $(DEFINES)
COMPILE_CXX         = $(CXX) $(CXXFLAGS) $(INC) $(DEFINES)

# Automatically include all source files
SRCS_C             := $(shell find $(SRC_DIR) -type f -name '*.c' -not -path '*/\.*')
SRCS_CXX           := $(shell find $(SRC_DIR) -type f -name '*.cpp' -not -path '*/\.*')
OBJECTS_C          := $(patsubst $(SRC_DIR)%,$(BUILD_DIR)/%,$(SRCS_C:.c=.$(OBJ_EXT)))
OBJECTS_CXX        := $(patsubst $(SRC_DIR)%,$(BUILD_DIR)/%,$(SRCS_CXX:.cpp=.$(OBJ_EXT)))
OBJECTS            := $(OBJECTS_C) $(OBJECTS_CXX)

all: default

$(BUILD_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.c
	@echo 'Building C file: $<'
	@mkdir -p $(dir $@)
	@$(COMPILE_C) -c -o "$@" "$<"

$(BUILD_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.cpp
	@echo 'Building C++ file: $<'
	@mkdir -p $(dir $@)
	@$(COMPILE_CXX) -c -o "$@" "$<"

default: $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $(BIN) $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf $(WORKING_DIR)