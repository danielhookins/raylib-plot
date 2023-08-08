# Raylib C

## Build

Note: You may need to change the path to the raylib library. This is using raylib installed via homebrew on macOS (version 4.5.0).

```
mkdir bin
clang -o bin/app src/main.c -I/opt/homebrew/Cellar/raylib/4.5.0/include/ -L/opt/homebrew/Cellar/raylib/4.5.0/lib/ -lraylib -framework IOKit -framework Cocoa -framework OpenGL
```

## Run

```
./bin/app
```

## Usage

Drag a csv file on to the window (see the example) to plot the data.
