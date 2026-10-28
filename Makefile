APP_ID ?= demo_kit
APP_NAME ?= Demo Kit

SDK_DIR ?= sdk
BUILD_DIR ?= build/$(APP_ID)
DIST_DIR ?= dist/$(APP_ID).app
DIST_TAR ?= dist/$(APP_ID).app.tar
PORT ?= 8000
APP_SRC_ROOT := examples/$(APP_ID)/src
APP_RES_ROOT := examples/$(APP_ID)/resources

CC := x86_64-elf-gcc
# The flags PeOS builds its own apps with (but -Werror).
CFLAGS ?= -std=gnu11 -O2 -Wall -Wextra -ffreestanding -fno-stack-protector -fno-pie -Wa,--noexecstack -mno-red-zone -mcmodel=kernel
# A PeOS source tree, for sync-sdk and peos-add: the one the kit sits in, or
# a sibling checkout.
PEOS ?= $(if $(wildcard ../userland/apps),..,../peos-starlight)

APP_SRC_C := $(shell find $(APP_SRC_ROOT) -type f -name '*.c' 2>/dev/null)
APP_SRC_S := $(shell find $(APP_SRC_ROOT) -type f -name '*.S' 2>/dev/null)
APP_SRC_s := $(shell find $(APP_SRC_ROOT) -type f -name '*.s' 2>/dev/null)
APP_SRCS := $(APP_SRC_C) $(APP_SRC_S) $(APP_SRC_s)
APP_OBJS := $(patsubst $(APP_SRC_ROOT)/%,$(BUILD_DIR)/%,$(APP_SRCS))
APP_OBJS := $(APP_OBJS:.c=.o)
APP_OBJS := $(APP_OBJS:.S=.o)
APP_OBJS := $(APP_OBJS:.s=.o)
APP_DEPS := $(APP_OBJS:.o=.d)
APP_MANIFEST := examples/$(APP_ID)/bundle/app.ini
SDK_CRT0 := $(SDK_DIR)/crt0.S
SDK_LINKER := $(SDK_DIR)/linker.ld
SDK_LIB := $(SDK_DIR)/lib/libpeos.a
SDK_INCLUDE := $(SDK_DIR)/include
BUILD_CRT0 := $(BUILD_DIR)/crt0.o
DIST_BIN := $(DIST_DIR)/app.bin
DIST_MANIFEST := $(DIST_DIR)/app.ini
APP_RES_FILES := $(shell find $(APP_RES_ROOT) -type f ! -name '.DS_Store' 2>/dev/null)
DIST_RES_FILES := $(patsubst $(APP_RES_ROOT)/%,$(DIST_DIR)/resources/%,$(APP_RES_FILES))

.PHONY: help check-env scaffold check-sources build package pack serve clean sync-sdk peos-add

help:
	@echo "Starlight App Kit (standalone-first)"
	@echo "  make scaffold APP_NAME='My App' APP_ID=myapp"
	@echo "  make build APP_ID=myapp                     # standalone: dist/myapp.app"
	@echo "  make package APP_ID=myapp                   # alias of build"
	@echo "  make serve APP_ID=myapp [PORT=8000]          # serve it: in PeOS, appget <url> installs it"
	@echo "  make pack APP_ID=myapp                      # dist/myapp.app.tar, what serve serves"
	@echo "  make sync-sdk PEOS=../peos-starlight         # refresh sdk/ from a PeOS tree"
	@echo "  make peos-add APP_ID=myapp PEOS=../peos-starlight  # add the app to a PeOS tree's image"

check-env:
	python3 scripts/check_env.py --sdk-dir "$(SDK_DIR)"

scaffold:
	python3 scripts/new_app.py --name "$(APP_NAME)" --id "$(APP_ID)"

check-sources:
	@test -n "$(APP_SRCS)" || (echo "No sources found under $(APP_SRC_ROOT)" && exit 1)

$(BUILD_CRT0): $(SDK_CRT0)
	@mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) -I"$(SDK_INCLUDE)" -c "$<" -o "$@"

$(BUILD_DIR)/%.o: $(APP_SRC_ROOT)/%.c
	@mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) -I"$(SDK_INCLUDE)" -MMD -MP -c "$<" -o "$@"

$(BUILD_DIR)/%.o: $(APP_SRC_ROOT)/%.S
	@mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) -I"$(SDK_INCLUDE)" -MMD -MP -c "$<" -o "$@"

$(BUILD_DIR)/%.o: $(APP_SRC_ROOT)/%.s
	@mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) -I"$(SDK_INCLUDE)" -MMD -MP -c "$<" -o "$@"

$(DIST_BIN): check-sources $(APP_OBJS) $(BUILD_CRT0) $(SDK_LIB) $(SDK_LINKER)
	@mkdir -p "$(DIST_DIR)"
	$(CC) $(CFLAGS) -I"$(SDK_INCLUDE)" -nostdlib -ffreestanding -fno-pie \
	    -Wl,-T,"$(SDK_LINKER)" -Wl,-z,max-page-size=0x1000 -Wl,-z,noexecstack \
	    "$(BUILD_CRT0)" $(APP_OBJS) "$(SDK_LIB)" -o "$@"

$(DIST_MANIFEST): $(APP_MANIFEST)
	@mkdir -p "$(DIST_DIR)"
	cp "$<" "$@"

$(DIST_DIR)/resources/%: $(APP_RES_ROOT)/%
	@mkdir -p "$(dir $@)"
	cp "$<" "$@"

build: $(DIST_BIN) $(DIST_MANIFEST) $(DIST_RES_FILES)
	@echo "Standalone bundle ready: $(DIST_DIR)"

package: build

# The bundle as one tar, for PeOS's appget: no rebuild of PeOS needed.
pack: build
	python3 scripts/pack_app.py "$(DIST_DIR)" "$(DIST_TAR)"

# Serves the tar on PORT and prints the appget command to type in the VM.
serve: pack
	python3 scripts/serve_app.py "$(DIST_TAR)" --port "$(PORT)"

sync-sdk:
	python3 scripts/sync_sdk.py --peos "$(PEOS)"

# The app's project goes into PeOS's userland/apps/, where the image build
# compiles it like the system apps (make -C $(PEOS) run-hdd to try it).
peos-add: check-sources
	@test -d "$(PEOS)/userland/apps" || (echo "Not a PeOS tree: $(PEOS)" && exit 1)
	rm -rf "$(PEOS)/userland/apps/$(APP_ID)"
	mkdir -p "$(PEOS)/userland/apps/$(APP_ID)"
	cp -R "examples/$(APP_ID)/src" "examples/$(APP_ID)/bundle" "$(PEOS)/userland/apps/$(APP_ID)/"
	if [ -d "$(APP_RES_ROOT)" ]; then cp -R "$(APP_RES_ROOT)" "$(PEOS)/userland/apps/$(APP_ID)/"; fi
	@echo "Added to $(PEOS)/userland/apps/$(APP_ID)"

clean:
	rm -rf "$(BUILD_DIR)" "$(DIST_DIR)" "$(DIST_TAR)"

-include $(APP_DEPS)
