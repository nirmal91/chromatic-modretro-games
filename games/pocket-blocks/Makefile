GBDK_HOME ?= /opt/gbdk
LCC := $(GBDK_HOME)/bin/lcc
.PHONY: all clean
all: dist/pocket-blocks.gbc
src/art.h: tools/make_art.py
	python3 tools/make_art.py
dist/pocket-blocks.gbc: src/main.c src/art.h
	mkdir -p dist
	"$(LCC)" -Wm-yc -Wm-ynPOCKET_BLOCKS -Wl-m -Wl-j -o "$@" src/main.c
clean:
	rm -f dist/pocket-blocks.gbc dist/pocket-blocks.map dist/pocket-blocks.noi
