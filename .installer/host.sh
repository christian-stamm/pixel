#!/bin/bash

echo "Pico Setup..."
echo "Configure Git Repo..."
git submodule init
git submodule sync --recursive
git submodule update --recursive --init 

# echo "Configure USB access..."
# sudo cp ./openocd/contrib/60-openocd.rules" "/etc/udev/rules.d/"
# sudo udevadm control --reload-rules && sudo udevadm trigger # restart udev

echo "Setup done."