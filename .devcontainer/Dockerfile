FROM ubuntu:noble

# Install DEPS
ARG GIT_DEPS="git"
ARG CPP_DEPS="libeigen3-dev libhidapi-dev clang-format clangd"
ARG PYT_DEPS="python3 python3-pip python3-dev python3-venv"
ARG BUILD_DEPS="gcc-arm-none-eabi build-essential cmake"
ARG PKG_DEPS="pkg-config automake autoconf checkinstall"
ARG USB_DEPS="libtool libftdi-dev libusb-1.0-0 libusb-1.0-0-dev usbutils udev"
ARG DBG_DEPS="gdb-multiarch"
ARG ALL_DEPS="$GIT_DEPS $CPP_DEPS $PYT_DEPS $BUILD_DEPS $PKG_DEPS $USB_DEPS $DBG_DEPS"

RUN apt update && \
    apt upgrade -y && \ 
    apt install -y $ALL_DEPS


# Configure ENV PATHs
ARG WORKSPACE_PATH="/workspaces/pixel"
ARG TOOLCHAIN_PATH="/devel/pixel/pico-toolchain"
ARG OPENOCD_PARAMS="--enable-ftdi --enable-sysfsgpio --enable-picoprobe --enable-cmsis-dap"

ENV PICO_SDK_PATH="$TOOLCHAIN_PATH/pico-sdk"
ENV PICO_EXTRAS_PATH="$TOOLCHAIN_PATH/pico-extras"
ENV PICO_EXAMPLES_PATH="$WORKSPACE_PATH/pico-examples"
ENV PICO_PLAYGROUND_PATH="$WORKSPACE_PATH/pico-playground"

# Download Tools
WORKDIR "$TOOLCHAIN_PATH"
RUN git clone -b master https://github.com/raspberrypi/pico-extras.git "$TOOLCHAIN_PATH/pico-extras" --depth=1 --recurse-submodules 
RUN git clone -b master https://github.com/raspberrypi/pico-sdk.git "$TOOLCHAIN_PATH/pico-sdk" --depth=1 --recurse-submodules 
RUN git clone -b master https://github.com/raspberrypi/picotool.git "$TOOLCHAIN_PATH/picotool" --depth=1 --recurse-submodules 
RUN git clone -b rp2040 https://github.com/raspberrypi/openocd.git "$TOOLCHAIN_PATH/picodebug" --depth=1 --recurse-submodules

# Install Tools
WORKDIR "$TOOLCHAIN_PATH/picodebug"
RUN ./bootstrap && ./configure $OPENOCD_PARAMS && make -j 8 install
WORKDIR "$TOOLCHAIN_PATH/picotool"
RUN mkdir -p build && cd build && cmake ../ && make -j 8 && cp picotool /usr/local/bin

WORKDIR "$WORKSPACE_PATH"