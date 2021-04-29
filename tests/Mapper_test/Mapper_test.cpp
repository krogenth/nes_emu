#include <filesystem>	//	std::filesystem
#include <iostream>		//	std::cout, std::cerr, std::clog
#include <iomanip>
#include <assert.h>		//	assert()

#include "..\..\src\include\Mapper_Collection.h"
#include "..\..\src\include\Cartridge.h"
#include "..\..\src\include\RomStruct.h"

#include "..\..\src\include\custom_exceptions.h"

int main(int argc, char* argv[]) {

	//	store the current working directory
	std::string directory = std::filesystem::current_path().u8string();
	std::cout << "The current directory is: " << directory << '\n';
	std::cout << "nesMagic String: " << nesMagic << '\n';

	if (!std::filesystem::exists(directory + "\\roms")) {

		std::cout << "Failed to locate " << directory << "\\roms please make sure the directory exists and roms are located there to test.\n" << std::flush;
		std::cout << "Press enter to continue...\n" << std::flush;
		std::cin.get();
		return EXIT_SUCCESS;

	}

	CartridgeClass cartridge;
	romStruct* rom = nullptr;
	MapperClass* mapper = nullptr;

	//	for-each loop through all files within directory\\roms
	for (const auto& p : std::filesystem::directory_iterator(directory + "\\roms")) {

		//	store path string so we can check it
		std::string filename = p.path().u8string();

		//size_t stringPos = ;
		std::string relativePath = filename.substr(filename.rfind("\\") + 1);

		//	we only want to test .nes files
		if (filename.find(".nes") == std::string::npos)
			continue;

		try {

			rom = cartridge.load(filename);
			cartridge.storeMapper(selectMapper(rom));

			uint32_t mapperID = (rom->header.flags7 & 0b11110000) | ((rom->header.flags6 & 0b11110000) >> 4);
			std::cout << std::setw(65) << relativePath << " MapperID " << mapperID << " supported\n" << std::flush;

			delete mapper;

		}
		catch (const MapperException& e) {

			std::cout << std::setw(65) << relativePath << " " << e.what() << '\n' << std::flush;

		}

		rom = nullptr;
		mapper = nullptr;

	}

	std::cout << "Press enter to continue...\n" << std::flush;
	std::cin.get();

	return EXIT_SUCCESS;

}