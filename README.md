# Gameboy Emulator
A simple gameboy emulator im writing for learning more about emulating in general. It is not a perfect replica of the gameboy, and has some bugs and timing issues, so a couple games may not work with it. 

## Usage
### Requirements
To build the project `cmake` is required. Other libraries requirements are included in thirdparty folder. This project was only tested on Windows.

### Building
```
$ cmake -S. -Bbuild/
$ cmake --build build/
```

## Input
Z - A button

X - B button

C - SELECT button

V - START button

Arrow keys - UP/DOWN/LEFT/RIGHT buttons

Hold Alt to show menu bar

## Features
- Correct CPU instructions (passes all blargg's cpu instrs tests)
- PPU Background/Window/Sprite rendering
- MBC support (Only MBC1 is implemented)
- Debug tools (VRAM viewer / CPU Disassembly / Breakpoints)

**_NOTE:_** At the moment this emulator still lacks alot of stuff, and I probably won't implement it since I barely work on this project anymore, but i still plan on implementing sounds and maybe fixing some issues in the future. 