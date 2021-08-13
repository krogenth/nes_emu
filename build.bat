cd .\libs\sfml
mkdir .\build
cd .\build
cmake -G "Visual Studio 16 2019" ..\
MSBuild SFML.sln /property:configuration=Debug /property:Platform=x64
MSBuild SFML.sln /property:configuration=Release /property:Platform=x64
cd ..\..\Nes_Snd_Emu
mkdir .\build
cd .\build
cmake -G "Visual Studio 16 2019" ..\
MSBuild Nes_Snd_Emu.sln /property:configuration=Debug /property:Platform=x64
MSBuild Nes_Snd_Emu.sln /property:configuration=Release /property:Platform=x64
cd ..\..\..
MSBuild .\vs\nes.sln /property:configuration=Debug /property:Platform=x64
MSBuild .\vs\nes.sln /property:configuration=Release /property:Platform=x64