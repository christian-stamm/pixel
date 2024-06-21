#!/bin/bash

WORKSPACE_PATH="$PWD"
TOOLCHAIN_PATH="$WORKSPACE_PATH/pico-toolchain"

echo "Pico Setup..."

echo "Configure Git Repo..."
git submodule init
git submodule sync --recursive
git submodule update --recursive --init 

echo "Configure USB access..."
sudo wget https://raw.githubusercontent.com/raspberrypi/openocd/rp2040/contrib/60-openocd.rules -O "/etc/udev/rules.d/60-openocd.rules"
sudo udevadm control --reload-rules 
sudo udevadm trigger # restart udev

echo "Setup done."