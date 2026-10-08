GBDK_HOME ?= /opt/gbdk
LCC := $(GBDK_HOME)/bin/lcc
.PHONY: all art clean
all: dist/water-rings.gbc
art: src/art.h
src/art.h: tools/make_art.py
	python3 tools/make_art.py
dist/water-rings.gbc: src/main.c src/art.h Makefile
	mkdir -p dist
	"$(LCC)" -Wm-yc -Wm-ynWATER_RINGS -o "$@" src/main.c
clean:
	rm -f dist/water-rings.gbc
