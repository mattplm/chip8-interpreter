#!/usr/bin/env bash

# Who needs a Makefile?

gcc `sdl2-config --libs --cflags` -O3 src/*.c -o chip8 -Iinclude/
