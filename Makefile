APP_ID ?= demo_kit
APP_NAME ?= Demo Kit

SDK_DIR ?= sdk
BUILD_DIR ?= build/$(APP_ID)
DIST_DIR ?= dist/$(APP_ID).app
APP_SRC_ROOT := examples/$(APP_ID)/src

CC := x86_64-elf-gcc
CFLAGS ?= -std=gnu11 -O2 -Wall -Wextra -ffreestanding -fno-stack-protector -fno-pie -mno-red-zone -mcmodel=kernel

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

.PHONY: help check-env scaffold check-sources build package clean

help:
	@echo "Starlight App Kit (standalone-first)"
	@echo "  make scaffold APP_NAME='My App' APP_ID=myapp"
	@echo "  make build APP_ID=myapp                     # standalone: dist/myapp.app"
	@echo "  make package APP_ID=myapp                   # alias of build"

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
	    -Wl,-T,"$(SDK_LINKER)" -Wl,-z,max-page-size=0x1000 \
	    "$(BUILD_CRT0)" $(APP_OBJS) "$(SDK_LIB)" -o "$@"

$(DIST_MANIFEST): $(APP_MANIFEST)
	@mkdir -p "$(DIST_DIR)"
	cp "$<" "$@"

build: $(DIST_BIN) $(DIST_MANIFEST)
	@echo "Standalone bundle ready: $(DIST_DIR)"

package: build

clean:
	rm -rf "$(BUILD_DIR)" "$(DIST_DIR)"

-include $(APP_DEPS)
