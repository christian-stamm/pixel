#!/bin/bash

WORKSPACE_PATH="$PWD"
TOOLCHAIN_PATH="$WORKSPACE_PATH/pico-toolchain"

echo "Pico Setup..."

echo "Configure Git Repo..."
git submodule init
git submodule sync --recursive
git submodule update --recursive --init 

echo "Host Setup done."