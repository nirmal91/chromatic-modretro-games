GBDK_HOME ?= $(HOME)/Library/Application Support/modretro-chromatic/toolchain/.local/gbdk
LCC := $(GBDK_HOME)/bin/lcc
ROOT := ..
SLUGS := cargo-crush storm-riders parcel-panic lost-drones airlock-alert
.PHONY: all clean
all: $(foreach slug,$(SLUGS),$(ROOT)/$(slug)/dist/$(slug).gbc)
$(ROOT)/cargo-crush/dist/cargo-crush.gbc: src/main.c
	"$(LCC)" -Wm-yC -Wm-ynCARGO_CRUSH -DGAME_ID=1 -o "$@" "$<"
$(ROOT)/storm-riders/dist/storm-riders.gbc: src/main.c
	"$(LCC)" -Wm-yC -Wm-ynSTORM_RIDERS -DGAME_ID=2 -o "$@" "$<"
$(ROOT)/parcel-panic/dist/parcel-panic.gbc: src/main.c
	"$(LCC)" -Wm-yC -Wm-ynPARCEL_PANIC -DGAME_ID=3 -o "$@" "$<"
$(ROOT)/lost-drones/dist/lost-drones.gbc: src/main.c
	"$(LCC)" -Wm-yC -Wm-ynLOST_DRONES -DGAME_ID=4 -o "$@" "$<"
$(ROOT)/airlock-alert/dist/airlock-alert.gbc: src/main.c
	"$(LCC)" -Wm-yC -Wm-ynAIRLOCK_ALERT -DGAME_ID=5 -o "$@" "$<"
clean:
	rm -f $(foreach slug,$(SLUGS),$(ROOT)/$(slug)/dist/$(slug).gbc)
