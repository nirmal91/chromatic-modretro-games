GBDK_HOME ?= $(HOME)/Library/Application Support/modretro-chromatic/toolchain/.local/gbdk
LCC := $(GBDK_HOME)/bin/lcc

.PHONY: all clean
all: dist/cosmic-chess-academy.gbc

src/pieces.h: tools/make_pieces.py
	python3 tools/make_pieces.py

dist/cosmic-chess-academy.gbc: src/main.c src/pieces.h ../meteor-mayhem/src/art.h
	mkdir -p dist
	"$(LCC)" -Wm-yC -Wm-ynCOSMIC_CHESS -o "$@" src/main.c

clean:
	rm -f dist/cosmic-chess-academy.gbc
