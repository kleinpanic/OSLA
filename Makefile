# File: Makefile
# Smart Makefile for building and installing OSLA on Linux/macOS

UNAME_S := $(shell uname -s)

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
LDFLAGS =

# If Linux, try static linking (optional)
ifeq ($(UNAME_S), Linux)
    STATIC_LINKING := 1
    DEFAULT_PREFIX := /usr/local
else
    STATIC_LINKING := 0
    DEFAULT_PREFIX := $(HOME)/.local
endif

# Overrideable PREFIX (via command line)
PREFIX ?= $(DEFAULT_PREFIX)

# Derived install directories
BINDIR := $(PREFIX)/bin
DATADIR := $(PREFIX)/share/osla
CONFDIR := $(HOME)/.config/OSLA

SRC_DIR = src
OBJ_DIR = obj
BIN = osla

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: all clean install uninstall local-env

all: $(BIN)

$(BIN): $(OBJS)
ifeq ($(STATIC_LINKING), 1)
	$(CC) $(CFLAGS) -static -o $@ $^ $(LDFLAGS)
else
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
endif

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)

install: $(BIN)
	@echo "Installing to: $(BINDIR)"
	@mkdir -p $(BINDIR) $(DATADIR)/licenses $(DATADIR)/descriptions "$(CONFDIR)"
	@cp $(BIN) $(BINDIR)/
	@cp -r licenses/* $(DATADIR)/licenses/
	@cp -r descriptions/* $(DATADIR)/descriptions/
	@cp config/osla.conf "$(CONFDIR)/osla.conf"
	@echo "Installation complete."

uninstall:
	@echo "Uninstalling..."
	@rm -f $(BINDIR)/$(BIN)
	@rm -rf $(DATADIR)/licenses $(DATADIR)/descriptions
	@rm -rf "$(CONFDIR)"
	@echo "Uninstall complete."

local-env:
	@echo "export OSLA_DATADIR=`pwd`"

