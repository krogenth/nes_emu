#include <filesystem>	//	std::filesystem
#include <iostream>		//	std::cout, std::cerr, std::clog
#include <assert.h>		//	assert()

#include "..\..\src\include\Mapper_Collection.h"
#include "..\..\src\include\Cartridge.h"
#include "..\..\src\include\RomStruct.h"

#include "..\..\src\include\custom_exceptions.h"

int main(int argc, char* argv[]) {

	std::cout << "The current directory is: " << std::filesystem::current_path() << '\n';
	CartridgeClass Cartridge;
	romStruct* rom = Cartridge.load("test.nes");
	MapperClass* mapper = nullptr;
	
	try {

		mapper = selectMapper(rom);
		rom = nullptr;	//	since the mapper was successful, we can just set this to nullptr, mapper will delete the pointer upon destruction

	}
	catch (const MapperException& e) {

		std::cerr << e.what() << '\n';

	}

	//	since the Cartridge deletes the mapper, we can just set the mapper pointer to nullptr here
	if (mapper != nullptr)
		mapper = nullptr;
	
	if (rom != nullptr)
		rom = nullptr;

	std::cin.get();

	return EXIT_SUCCESS;

}