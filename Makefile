# SPDX-License-Identifier: MIT
# Copyright (c) 2020-2026 RVX Project Contributors

SHELL := /usr/bin/env bash

BUILD_DIR ?= build
CMAKE_BUILD_TYPE ?= Release
CMAKE_ARGS ?=
PREFIX_FILE := $(BUILD_DIR)/.install_prefix
BUILD_STAMP := $(BUILD_DIR)/.build_done

.PHONY: all build install uninstall clean prepare-prefix

all: build

prepare-prefix:
	@selected_prefix="$(PREFIX)"; \
	if [[ -z "$$selected_prefix" && -f "$(PREFIX_FILE)" ]]; then \
		selected_prefix="$$(cat "$(PREFIX_FILE)")"; \
	fi; \
	if [[ -z "$$selected_prefix" ]]; then \
		selected_prefix="/usr/local"; \
	fi; \
	mkdir -p "$(BUILD_DIR)"; \
	printf '%s\n' "$$selected_prefix" > "$(PREFIX_FILE)"

build: 
	@echo "Configuring RVX SDK..."; \
	cmake -S . -B "$(BUILD_DIR)" \
		-DCMAKE_BUILD_TYPE="$(CMAKE_BUILD_TYPE)" \
		$(CMAKE_ARGS); \
	echo "Building RVX SDK..."; \
	cmake --build "$(BUILD_DIR)"; \
	echo "RVX SDK build completed."; \
	echo "Run 'make install' for a system-wide installation (PREFIX=/usr/local)."; \
	echo "Run 'make install PREFIX=/install/path' to install the SDK to a custom path."; \
	touch "${BUILD_STAMP}"

check-build:
	@if [[ ! -f "$(BUILD_STAMP)" ]]; then \
			echo "Build step has not been run yet." >&2; \
			echo "Run 'make build' first." >&2; \
			exit 1; \
	fi

install: check-build prepare-prefix
	@selected_prefix="$$(cat "$(PREFIX_FILE)")"; \
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
	echo "RVX SDK install completed."

uninstall:
	@selected_prefix="$(PREFIX)"; \
	if [[ -z "$$selected_prefix" && -f "$(PREFIX_FILE)" ]]; then \
		selected_prefix="$$(cat "$(PREFIX_FILE)")"; \
	fi; \
	if [[ -z "$$selected_prefix" ]]; then \
		selected_prefix="/usr/local"; \
	fi; \
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
	build_manifest="$(BUILD_DIR)/install_manifest.txt"; \
	build_uninstall="$(BUILD_DIR)/cmake_uninstall.cmake"; \
	prefix_manifest_lib="$$selected_prefix/lib/cmake/RVX/RVXInstallManifest.txt"; \
	prefix_uninstall_lib="$$selected_prefix/lib/cmake/RVX/cmake_uninstall.cmake"; \
	prefix_manifest_lib64="$$selected_prefix/lib64/cmake/RVX/RVXInstallManifest.txt"; \
	prefix_uninstall_lib64="$$selected_prefix/lib64/cmake/RVX/cmake_uninstall.cmake"; \
	if [[ -f "$$build_manifest" ]]; then \
		if [[ ! -f "$$build_uninstall" ]]; then \
			echo "Found build manifest, but missing script: $$build_uninstall" >&2; \
			exit 1; \
		fi; \
		echo "Using build manifest: $$build_manifest"; \
		cmake -P "$$build_uninstall"; \
		exit 0; \
	fi; \
	if [[ -f "$$prefix_manifest_lib" ]]; then \
		if [[ ! -f "$$prefix_uninstall_lib" ]]; then \
			echo "Found installed manifest, but missing script: $$prefix_uninstall_lib" >&2; \
			exit 1; \
		fi; \
		echo "Using installed manifest: $$prefix_manifest_lib"; \
		cmake -P "$$prefix_uninstall_lib"; \
		exit 0; \
	fi; \
	if [[ -f "$$prefix_manifest_lib64" ]]; then \
		if [[ ! -f "$$prefix_uninstall_lib64" ]]; then \
			echo "Found installed manifest, but missing script: $$prefix_uninstall_lib64" >&2; \
			exit 1; \
		fi; \
		echo "Using installed manifest: $$prefix_manifest_lib64"; \
		cmake -P "$$prefix_uninstall_lib64"; \
		exit 0; \
	fi; \
	echo "RVX SDK uninstall manifest not found. It may have already been uninstalled." >&2; \
	echo "Checked build: $$build_manifest" >&2; \
	echo "Checked install: $$prefix_manifest_lib" >&2; \
	echo "Checked install: $$prefix_manifest_lib64" >&2; \
	exit 1

clean:
	@rm -rf "$(BUILD_DIR)"