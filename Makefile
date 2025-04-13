# File: Makefile
# Makefile for building the OSLA (Open-source Licence Agreements) project.
# Targets: all, clean, install, uninstall, local-env

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
LDFLAGS =
SRC_DIR = src
OBJ_DIR = obj
BIN = osla

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Installation directories (global install defaults)
PREFIX ?= /usr/local
BINDIR := $(PREFIX)/bin
DATADIR := $(PREFIX)/share/osla

# For configuration, we install to the user's config directory.
# This installs to $(HOME)/.config/OSLA, so it uses the invoking user's home directory.
CONFDIR := $(HOME)/.config/OSLA

.PHONY: all clean install uninstall local-env

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -static -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)

install: $(BIN)
	# Install binary to system directory (requires sudo)
	sudo install -Dm755 $(BIN) $(BINDIR)/$(BIN)
	# Install data: licenses and descriptions (requires sudo)
	sudo install -d $(DATADIR)/licenses
	sudo install -d $(DATADIR)/descriptions
	sudo cp -r licenses/* $(DATADIR)/licenses/
	sudo cp -r descriptions/* $(DATADIR)/descriptions/
	# Install default configuration file into the user's config directory (no sudo)
	mkdir -p $(CONFDIR)
	cp config/osla.conf $(CONFDIR)/osla.conf

uninstall:
	sudo rm -f $(BINDIR)/$(BIN)
	sudo rm -rf $(DATADIR)/licenses $(DATADIR)/descriptions
	rm -rf $(CONFDIR)

# Target to print environment export commands for local builds.
# Use: source <(make local-env)
local-env:
	@echo "export OSLA_DATADIR=`pwd`"

