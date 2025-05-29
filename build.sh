#!/bin/sh

clang src/sdl_ogl.c -o ogl -lSDL3 -lm -pthread
