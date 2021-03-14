# NES_EMU

An Open Source NES Emulator using C++17.

## Build Status
[![AppVeyor build status](https://ci.appveyor.com/api/projects/status/8yhgowh6j3fxwlo1/branch/main?svg=true)](https://ci.appveyor.com/project/krogenth/nes-emu/branch/main)

## How to build

first fetch the source code:

    git clone https://github.com/krogenth/nes_emu.git
    
    cd nes_emu
    
    git submodule update --init --recursive

### Windows with Visual Studio

Open the .sln file contained in ./vs/

Build desired projects

All executables will be in ./vs/x64/Release or ./vs/x64/Debug
