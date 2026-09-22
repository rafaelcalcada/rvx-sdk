# SPDX-License-Identifier: MIT
# Copyright (c) 2020-2026 RVX Project Contributors

SHELL := /usr/bin/env bash

BUILD_DIR ?= build
CMAKE_BUILD_TYPE ?= Release
CMAKE_ARGS ?=
PREFIX ?= $(HOME)/.local/rvx-sdk
BUILD_STAMP := $(BUILD_DIR)/.build_done

.PHONY: all build install uninstall clean

all: build

build: $(BUILD_STAMP)

$(BUILD_STAMP):
	@set -e; \
	echo "Configuring RVX SDK..."; \
	cmake -S . -B "$(BUILD_DIR)" \
		-DCMAKE_BUILD_TYPE="$(CMAKE_BUILD_TYPE)" \
		$(CMAKE_ARGS); \
	echo "Building RVX SDK..."; \
	cmake --build "$(BUILD_DIR)" -- -s --no-print-directory; \
	echo "RVX SDK build completed."; \

ifneq ($(filter install,$(MAKECMDGOALS)),install)
	@set -e; \
	echo "Run 'make install' to install it locally for the current user (PREFIX=\$$HOME/.local/rvx-sdk)."; \
	echo "Run 'make install PREFIX=/install/path' to install the SDK to a custom path (e.g. /opt/rvx-sdk)."; \
	
endif
	@touch "$@"

install: build
	@selected_prefix="$(PREFIX)"; \
	check_path="$$selected_prefix"; \
	while [[ ! -e "$$check_path" && "$$check_path" != "/" ]]; do \
		check_path="$$(dirname "$$check_path")"; \
	done; \
	if [[ ! -w "$$check_path" ]]; then \
		echo "No write permission for '$$selected_prefix'. You may need to use 'sudo'." >&2; \
		  if [[ "$$selected_prefix" == "/usr/local" ]]; then \
        echo "Try 'sudo make install' or set PREFIX to a writable path." >&2; \
			else \
      	echo "Try 'sudo make install PREFIX=$$selected_prefix' or change PREFIX to a writable path." >&2; \
    	fi; \
		exit 1; \
	fi; \
	echo "Installing RVX SDK..."; \
	cmake --install "$(BUILD_DIR)" --prefix "$$selected_prefix"; \
	config_file="$$(grep -m1 'RVXConfig\.cmake$$' "$(BUILD_DIR)/install_manifest.txt")"; \
	if [[ -n "$$config_file" ]]; then \
		mkdir -p "$$HOME/.cmake/packages/RVX"; \
		dirname "$$config_file" > "$$HOME/.cmake/packages/RVX/rvx-sdk"; \
	fi; \
	echo "RVX SDK installation completed."

uninstall:
	@set -e; \
	selected_prefix="$(PREFIX)"; \
	check_path="$$selected_prefix"; \
	while [[ ! -e "$$check_path" && "$$check_path" != "/" ]]; do \
			check_path="$$(dirname "$$check_path")"; \
	done; \
	if [[ ! -w "$$check_path" ]]; then \
			echo "No write permission for '$$selected_prefix'. You may need to use 'sudo'." >&2; \
			if [[ "$$selected_prefix" == "/usr/local" ]]; then \
					echo "Try 'sudo make uninstall'." >&2; \
			else \
					echo "Try 'sudo make uninstall PREFIX=$$selected_prefix'." >&2; \
			fi; \
			exit 1; \
	fi; \
	manifest="$$selected_prefix/RVXInstallManifest.txt"; \
	if [[ ! -f "$$manifest" ]]; then \
		echo "RVX SDK install manifest not found. The RVX SDK may have already been uninstalled." >&2; \
		echo "Checked: $$manifest" >&2; \
		exit 1; \
	fi; \
	echo "Uninstalling RVX SDK from $$selected_prefix..."; \
	while IFS= read -r installed_file; do \
		[[ -n "$$installed_file" ]] || continue; \
		echo "Removing: $$installed_file"; \
		rm -f "$$installed_file"; \
	done < "$$manifest"; \
	if [[ -f "$$manifest" ]]; then \
		echo "Removing: $$manifest"; \
		rm -f "$$manifest"; \
	fi; \
	find "$$selected_prefix" -depth -type d -empty -printf 'Removing empty directory: %p\n' -delete 2>/dev/null || true; \
	rm -f "$$HOME/.cmake/packages/RVX/rvx-sdk"; \
	echo "RVX SDK uninstall completed."

clean:
	@rm -rf "$(BUILD_DIR)"