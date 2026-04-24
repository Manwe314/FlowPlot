SHELL := /bin/bash
.RECIPEPREFIX := >

PROJECT_NAME := FlowPlotGUI
BUILD_DIR := build
TARGET := FlowPlotGUI
BUILD_BIN := $(BUILD_DIR)/$(TARGET)
APP_BIN := ./$(TARGET)
VULKAN_ENV_SCRIPT := ./external/FlowUi/tools/scripts/vulkan_env.sh
VULKAN_SOURCE_CMD := source $(VULKAN_ENV_SCRIPT)
FLOWUI_DEV_CHANGE_UPDATER := ./external/FlowUi/flowui_devChange_updater
FLOWUI_OVERRIDES := ./.flowui/overrides.v1.json
JOBS ?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

.PHONY: all precheck configure build clean fclean re run rerun update

all: build

build: precheck $(BUILD_DIR)/CMakeCache.txt
> cmake --build $(BUILD_DIR) --target $(TARGET) -j$(JOBS)
> cp -f $(BUILD_BIN) $(APP_BIN)
> chmod +x $(APP_BIN)
> @echo "[make] Built executable: $(APP_BIN)"

configure: precheck $(BUILD_DIR)/CMakeCache.txt

precheck:
> @if [ -z "$$VULKAN_SDK" ]; then \
> 	cmd='$(VULKAN_SOURCE_CMD)'; \
> 	copied=0; \
> 	if command -v wl-copy >/dev/null 2>&1; then \
> 		printf "%s" "$$cmd" | wl-copy; copied=1; \
> 	elif command -v xclip >/dev/null 2>&1; then \
> 		printf "%s" "$$cmd" | xclip -selection clipboard; copied=1; \
> 	elif command -v xsel >/dev/null 2>&1; then \
> 		printf "%s" "$$cmd" | xsel --clipboard --input; copied=1; \
> 	elif command -v pbcopy >/dev/null 2>&1; then \
> 		printf "%s" "$$cmd" | pbcopy; copied=1; \
> 	fi; \
> 	echo "[make] Vulkan SDK is not sourced (VULKAN_SDK is empty)."; \
> 	if [ $$copied -eq 1 ]; then \
> 		echo "[make] Copied to clipboard: $$cmd"; \
> 	else \
> 		echo "[make] Clipboard utility not found. Run this command:"; \
> 		echo "       $$cmd"; \
> 	fi; \
> 	echo "[make] After sourcing Vulkan env, run make again."; \
> 	exit 1; \
> fi

$(BUILD_DIR)/CMakeCache.txt:
> cmake -S . -B $(BUILD_DIR)

run: build
> ./$(TARGET)

clean:
> rm -rf $(BUILD_DIR)
> @echo "[make] Removed $(BUILD_DIR) (kept $(APP_BIN) if present)."

fclean: clean
> rm -f $(APP_BIN)
> @echo "[make] Removed $(APP_BIN)."

re: fclean all

rerun: fclean run

update:
> $(FLOWUI_DEV_CHANGE_UPDATER) $(FLOWUI_OVERRIDES)
