GBDK_HOME ?= $(HOME)/Library/Application Support/modretro-chromatic/toolchain/.local/gbdk
LCC := $(GBDK_HOME)/bin/lcc

.PHONY: all clean art
all: dist/meteor-mayhem.gbc

art: src/art.h

src/art.h: tools/make_art.py
	python3 tools/make_art.py

dist/meteor-mayhem.gbc: src/main.c src/art.h
	mkdir -p dist
	"$(LCC)" -Wm-yC -Wm-ynMETEOR_MAYHEM -o "$@" src/main.c

clean:
	rm -f dist/meteor-mayhem.gbc
