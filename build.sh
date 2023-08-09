#!/bin/bash

clang -o bin/app src/main.c -I/opt/homebrew/Cellar/raylib/4.5.0/include/ -L/opt/homebrew/Cellar/raylib/4.5.0/lib/ -lraylib -framework IOKit -framework Cocoa -framework OpenGL