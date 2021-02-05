#include <filesystem>	//	std::filesystem
#include <iostream>		//	std::cout, std::cerr, std::clog
#include <assert.h>		//	assert()

#include "..\..\src\include\Cartridge.h"
#include "..\..\src\include\Mapper_Collection.h"
#include "..\..\src\include\RomStruct.h"

#include "..\..\src\include\custom_exceptions.h"

int main() {

	std::cout << "The current directory is: " << std::filesystem::current_path() << '\n';
	std::cout << "nesMagic String: " << nesMagic << '\n';
	CartridgeClass cartridge;

	Mapper* mapper = nullptr;

	try {

		mapper = cartridge.load("test.nes");

		//assert(nesMagic.compare(0, 4, rom->header.ID, 4) == 0);
		delete mapper;

	}
	catch (const CartridgeException& e) {

		std::cerr << e.what() << '\n';

	}

	return EXIT_SUCCESS;

}