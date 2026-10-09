GBDK_HOME ?= /opt/gbdk
LCC := $(GBDK_HOME)/bin/lcc
.PHONY: all clean
all: dist/pocket-jumper.gbc
src/art.h: tools/make_art.py
	python3 tools/make_art.py
dist/pocket-jumper.gbc: src/main.c src/art.h
	mkdir -p dist
	"$(LCC)" -Wm-yC -Wm-ynPOCKET_JUMPER -Wl-m -Wl-j -o "$@" src/main.c
clean:
	rm -f dist/pocket-jumper.gbc dist/pocket-jumper.map dist/pocket-jumper.sym
