#!/bin/bash

emscripten=false

while getopts "e" flag; do
    case "${flag}" in
        e) emscripten=true ;;
    esac
done

CC=cc
BIN=main

CFLAGS=(-Isrc -I../MibiEngine2/include ../MibiEngine2/build/MibiEngine2.a \
        -lm -lEGL -lX11 -lGL -lpng -Wall -Wextra -Wpedantic -ansi -g)

if $emscripten; then
    echo "-- Using emscripten!"
    CC=emcc
    CFLAGS=$CFLAGS" -sUSE_ZLIB=1 --embed-file build/assets@/"
    BIN=build/index.js
    mkdir -p build
    mkdir -p build/assets
    cp -r shaders build/assets/shaders
    cp -r assets build/assets/assets
    cp ../MibiEngine2/data/index.html build
fi

$CC src/*.c ${CFLAGS[@]} -o $BIN

