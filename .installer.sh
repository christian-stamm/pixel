#!/bin/bash

WORKSPACE_PATH="$PWD"
TOOLCHAIN_PATH="$WORKSPACE_PATH/pico-toolchain"

echo "Pico Setup..."

echo "Configure Git Repo..."
git submodule init
git submodule sync --recursive
git submodule update --recursive --init 

echo "Configure USB access..."
sudo cp "$TOOLCHAIN_PATH/picodebugger/contrib/60-openocd.rules" "/etc/udev/rules.d/"
sudo udevadm control --reload-rules 
sudo udevadm trigger # restart udev

echo "Setup done."