#include <filesystem>	//	std::filesystem
#include <iostream>		//	std::cout, std::cerr, std::clog
#include <assert.h>		//	assert()

#include "..\..\src\include\Cartridge.h"
#include "..\..\src\include\RomStruct.h"

#include "..\..\src\include\custom_exceptions.h"

int main() {

	std::cout << "The current directory is: " << std::filesystem::current_path() << '\n';
	std::cout << "nesMagic String: " << nesMagic << '\n';
	CartridgeClass cartridge;

	romStruct* rom = nullptr;

	try {

		rom = cartridge.load("test.nes");

		assert(nesMagic.compare(0, 4, rom->header.ID, 4) == 0);
		assert((rom->prg.size() % 0x4000) == 0);
		assert((rom->chr.size() % 0x2000) == 0);

		{

			std::cout << std::setw(30) << "prg_vector.size(): "          << rom->prg.size()                                                                << '\n';
			std::cout << std::setw(30) << "chr_vector.size(): "          << rom->chr.size()                                                                << '\n';

			std::cout << std::setw(30) << "prgRomSizeLSB: "              << (int)rom->header.prgRomSizeLSB                                                 << '\n';

			std::cout << std::setw(30) << "chrRomSizeLSB: "              << (int)rom->header.chrRomSizeLSB                                                 << '\n';

			std::cout << std::setw(30) << "mirroring: "                  << (rom->header.flags6 & 0b00000001)                                              << '\n';
			std::cout << std::setw(30) << "battery: "                    << ((rom->header.flags6 & 0b00000010) >> 1)                                       << '\n';
			std::cout << std::setw(30) << "trainer: "                    << ((rom->header.flags6 & 0b00000100) >> 2)                                       << '\n';
			std::cout << std::setw(30) << "four-screen: "                << ((rom->header.flags6 & 0b00001000) >> 3)                                       << '\n';

			std::cout << std::setw(30) << "mapperNum: "                  << ((rom->header.flags7 & 0b11110000) | ((rom->header.flags6 & 0b11110000) >> 4)) << '\n';
			std::cout << std::setw(30) << "console: "                    << (rom->header.flags7 & 0b00000011)                                              << '\n';
			std::cout << std::setw(30) << "NES2ID: "                     << ((rom->header.flags7 & 0b00001100) == 0b00001000)                              << '\n';

			std::cout << std::setw(30) << "f8.mapperNum: "               << (rom->header.flags8 & 0b00001111)                                              << '\n';
			std::cout << std::setw(30) << "f8.submapper: "               << ((rom->header.flags8 & 0b11110000) >> 4)                                       << '\n';

			std::cout << std::setw(30) << "prgRomSizeMSB: "              << (rom->header.prgChrSizeMSB & 0b00001111)                                       << '\n';
			std::cout << std::setw(30) << "chrRomSizeMSB: "              << (rom->header.prgChrSizeMSB & 0b11110000)                                       << '\n';

			std::cout << std::setw(30) << "prgRamSizeVolatileShift: "    << (rom->header.prgRamSize & 0b00001111)                                          << '\n';
			std::cout << std::setw(30) << "prgRamSizeNonVolatileShift: " << ((rom->header.prgRamSize & 0b11110000) >> 4)                                   << '\n';

			std::cout << std::setw(30) << "chrRamSizeVolatileShift: "    << (rom->header.chrRamSize & 0b00001111)                                          << '\n';
			std::cout << std::setw(30) << "chrRamSizeNonVolatileShift: " << ((rom->header.chrRamSize & 0b11110000) >> 4)                                   << '\n';

			std::cout << std::setw(30) << "CPU/PPU_Time: "               << (rom->header.timing & 0b00000011)                                              << '\n';

			std::cout << std::setw(30) << "PPUType: "                    << (rom->header.flags13 & 0b00001111)                                             << '\n';
			std::cout << std::setw(30) << "hardwareType: "               << ((rom->header.flags13 & 0b11110000) >> 4)                                      << '\n';

			std::cout << std::setw(30) << "miscROMs: "                   << (rom->header.miscROMs & 0b00000011)                                            << '\n';

			std::cout << std::setw(30) << "expansionDevice: "            << (rom->header.expandDevice & 0b00000011)                                        << '\n';

		}

		delete rom;

	}
	catch (const CartridgeException& e) {

		std::cerr << e.what() << '\n';

	}

	return EXIT_SUCCESS;

}