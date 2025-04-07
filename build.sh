#!/bin/bash

cc src/*.c -Isrc -I../MibiEngine2/include ../MibiEngine2/build/MibiEngine2.a \
   -lm -lEGL -lX11 -lGL -lpng -Wall -Wextra -Wpedantic -ansi -g -o main

