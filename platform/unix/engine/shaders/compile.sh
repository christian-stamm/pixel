#!/bin/bash

# Store the provided directory path
source="$1"
target="$2"

mkdir -p $source
mkdir -p $target

echo "GLSLC:" $source "->" $target

# Compile the shaders to SPIR-V
glslc -fshader-stage="vertex" "$source/vert.glsl"  -o "$target/vert.spv" 
if [ $? -ne 0 ]; then
    echo "GLSLC: Error compiling vertex shader"
    exit 1
fi

glslc -fshader-stage="fragment" "$source/frag.glsl"  -o "$target/frag.spv" 
if [ $? -ne 0 ]; then
    echo "GLSLC: Error compiling fragment shader"
    exit 1
fi

echo "Compilation successful"