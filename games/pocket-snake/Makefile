GBDK_HOME ?= /opt/gbdk
LCC := $(GBDK_HOME)/bin/lcc

.PHONY: all clean
all: dist/pocket-snake.gbc

src/art.h: tools/make_art.py
	python3 tools/make_art.py

dist/pocket-snake.gbc: src/main.c src/art.h
	mkdir -p dist
	"$(LCC)" -Wm-yc -Wm-ynPOCKET_SNAKE -o "$@" src/main.c

clean:
	rm -f dist/pocket-snake.gbc
