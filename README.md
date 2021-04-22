# NES_EMU

An Open Source NES Emulator using C++17.

## Build Status
[![AppVeyor build status](https://ci.appveyor.com/api/projects/status/8yhgowh6j3fxwlo1/branch/main?svg=true)](https://ci.appveyor.com/project/krogenth/nes-emu/branch/main)

## How to build

first ensure you have the tools required to compile:

Visual Studio 2019(https://visualstudio.microsoft.com/vs/)
cmake(https://cmake.org/)

second, open the "x64 Native Tools Command Prompt for VS 2019"

third, fetch the source code and libraries:

    git clone https://github.com/krogenth/nes_emu.git
    cd nes_emu
    git submodule update --init --recursive

last, compile the source code:

    cd libs/sfml
    mkdir build
    cd build
    cmake -G "Visual Studio 16 2019" ..\
    MSBuild SFML.sln /property:configuration=Release /property:Platform=x64
    cd ..\..\..
    MSBuild vs/nes.sln /property:configuration=Release /property:Platform=x64
	
if compiling this way is not desired, users may use the build.bat provided to automate the build instead

### Where to find the binaries

all binary files can be found within ./vs/x64/Release
