#!/bin/bash

# Exit on error
set -e

# Installation path (absolute)
workspacePath="$HOME/pixel"
toolchainPath="$workspacePath/rp2040"
openocdParams="--enable-cmsis-dap --enable-picoprobe --enable-bcm2835gpio"

#----------------------------------------------------------
#---------------------- Environment Setup -----------------
#----------------------------------------------------------

environment-setup()
{   
    echo "Workspace PATH=$workspacePath"
    
    # # Check system up to date
    sudo apt update
    sudo apt upgrade -y
    # Install dependencies
    echo "Installing Dependencies"
    GIT_DEPS="git"
    SDK_DEPS="cmake gcc-arm-none-eabi gcc-arm-linux-gnueabihf gcc g++"
    PICO_DEPS="libusb-1.0-0 libusb-1.0-0-dev pkg-config"
    OPENOCD_DEPS="gdb-multiarch automake autoconf build-essential texinfo libtool libftdi-dev libusb-1.0-0-dev libhidapi-dev checkinstall"
    VSCODE_DEPS="libx11-xcb1 libxcb-dri3-0 libdrm2 libgbm1 libegl-mesa0"
    DEPS="$GIT_DEPS $SDK_DEPS $PICO_DEPS $OPENOCD_DEPS $UART_DEPS $VSCODE_DEPS"
    sudo apt install -y $DEPS

    # Set git account variables
    echo "Setup git config"
    git config --global user.name "Christian Stamm"
    git config --global user.email "c.stamm1997@gmail.com"

    cd "$workspacePath"

    git submodule init
    git submodule sync --recursive
    git submodule update --recursive --init 

    # Install PATH_VARIABLES

    if [[ -v PICO_SDK_PATH ]]; then
        echo "\$PICO_SDK_PATH is already defined"
    else
        echo "Define \$PICO_SDK_PATH"
        DEST="$toolchainPath/pico-sdk"
        echo "export PICO_SDK_PATH=$DEST" >> ~/.bashrc
        export PICO_SDK_PATH=$DEST
    fi

    if [[ -v PICO_EXTRAS_PATH ]]; then
        echo "\$PICO_EXTRAS_PATH is already defined"
    else
        echo "Define \$PICO_EXTRAS_PATH"
        DEST="$toolchainPath/pico-extras"
        echo "export PICO_EXTRAS_PATH=$DEST" >> ~/.bashrc
        export PICO_EXTRAS_PATH=$DEST
    fi

    if [[ -v PICO_EXAMPLES_PATH ]]; then
        echo "\$PICO_EXAMPLES_PATH is already defined"
    else
        echo "Define \$PICO_EXAMPLES_PATH"
        DEST="$picoPath/pico-examples"
        echo "export PICO_EXAMPLES_PATH=$DEST" >> ~/.bashrc
        export PICO_EXAMPLES_PATH=$DEST
    fi
   
    if [[ -v PICO_PLAYGROUND_PATH ]]; then
        echo "\$PICO_PLAYGROUND_PATH is already defined"
    else
        echo "Define \$PICO_PLAYGROUND_PATH"
        DEST="$picoPath/pico-playground"
        echo "export PICO_PLAYGROUND_PATH=$DEST" >> ~/.bashrc
        export PICO_PLAYGROUND_PATH=$DEST
    fi

    source ~/.bashrc

    # Build Picoprobe
    echo "Building Picoprobe..."
    cd "$toolchainPath/picoprobe"

    if [ ! -d "$toolchainPath/picoprobe/build" ]; then
        echo "Create build directory..."
        mkdir build
    fi

    cd build
    cmake ../
    make

    # Build Picotool
    echo "Building Picotool..."
    cd "$toolchainPath/picotool"

    if [ ! -d "$toolchainPath/picotool/build" ]; then
        echo "Create build directory..."
        mkdir build
    fi

    cd build
    cmake ../
    make
    sudo cp picotool /usr/local/bin/

    # Build OpenOCD
    echo "Building OpenOCD..."
    cd "$toolchainPath/picodebug"
    OPENOCD_CONFIGURE_ARGS="--enable-ftdi --enable-sysfsgpio $openocdParams"
    ./bootstrap
    ./configure $OPENOCD_CONFIGURE_ARGS
    make
    sudo make install
    sudo cp "$toolchainPath/picodebug/contrib/60-openocd.rules" "/etc/udev/rules.d/"
    sudo udevadm control --reload-rules && sudo udevadm trigger # restart udev

    sudo apt install binutils-multiarch
    cd /usr/bin
    sudo ln -s /usr/bin/objdump objdump-multiarch
    sudo ln -s /usr/bin/nm nm-multiarch
    
    # Configure VSCode
    code --install-extension ms-vscode.cpptools
    code --install-extension ms-vscode.cpptools-extension-pack
    code --install-extension ms-vscode.cmake-tools
    code --install-extension ms-vscode.vscode-serial-monitor
    code --install-extension llvm-vs-code-extensions.vscode-clangd
    code --install-extension twxs.cmake
    code --install-extension marus25.cortex-debug

    # BCM27XX Setup
    # sudo add-apt-repository ppa:ubuntu-toolchain-r/test
    # sudo apt upgrade libstdc++6
}



echo "Init Setup."
environment-setup
echo "Setup done."
