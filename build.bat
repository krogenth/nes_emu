cd .\libs\sfml
mkdir .\build
cd .\build
cmake -G "Visual Studio 16 2019" ..\
MSBuild SFML.sln /property:configuration=Release /property:Platform=x64
cd ..\..\..
MSBuild .\vs\nes.sln /property:configuration=Release /property:Platform=x64