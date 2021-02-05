#include <fstream>	//	std::ifstream

#include "include\Cartridge.h"
#include "include\Mapper_Collection.h"
#include "include\RomStruct.h"

#include "include\io_util.h"
#include "include\custom_exceptions.h"

[[nodiscard]] Mapper* CartridgeClass::load(std::string filename) {

	//	following the nesdev.com documentation on the NES 2.0 header: https://wiki.nesdev.com/w/index.php/NES_2.0

	std::ifstream input(filename, std::ios::in | std::ios::binary);
	if (!input.is_open())
		throw CartridgeException("Cartridge file not found");

	romStruct* rom = new romStruct;
	rom->header = readRaw<romHeaderStruct>(input);

	//	ensure the file read is an NES rom by checking the ID
	//	if not, close file and delete pointer to remove memory leaks
	if (nesMagic.compare(0, 4, rom->header.ID, 4) != 0) {

		input.close();
		delete rom;
		throw CartridgeException("Cartridge not NES format");

	}

	/*
	to check the mirroring the NES rom uses, check:
		rom->header.flags6 & 0b00000001
		0 will be horizontal mirroring, 1 will be vertical mirroring
	*/

	//	check if this is an NES 2.0 rom: https://wiki.nesdev.com/w/index.php/NES_2.0
	//	this is according to the nesdev documentation, the bits should be 10 to be classified as NES 2.0
	if ((rom->header.flags7 & 0b00001100) == 0b00001000) {

		//	we want to store the number of chunks each segment has as well for later use with the Mapper
		rom->prg_chunks = (uint32_t)rom->header.prgRomSizeLSB | ((uint32_t)(rom->header.prgChrSizeMSB & 0b00001111) << 8);
		rom->chr_chunks = (uint32_t)rom->header.chrRomSizeLSB | ((uint32_t)(rom->header.prgChrSizeMSB & 0b11110000) << 4);
		rom->prg.resize((size_t)rom->prg_chunks * 0x4000);		//	0x4000 == 16KB
		rom->chr.resize((size_t)rom->chr_chunks * 0x2000);		//	0x2000 == 8KB

	}
	//	otherwise it's an iNES rom: https://wiki.nesdev.com/w/index.php/INES
	else {

		rom->prg_chunks = (uint32_t)rom->header.prgRomSizeLSB;
		rom->chr_chunks = (uint32_t)rom->header.chrRomSizeLSB;
		rom->prg.resize(((size_t)rom->header.prgRomSizeLSB * 0x4000));		//	0x4000 == 16KB
		rom->chr.resize(((size_t)rom->header.chrRomSizeLSB * 0x2000));		//	0x2000 == 8KB

	}

	//	read in prg and chr data into their respective vectors
	//	for the prg section, we need to ignore the header(16 bytes) and the possible "trainer" section(512 bytes)
	uint32_t prg_offset = 16 + (512 * ((rom->header.flags6 & 0b00000100) >> 2));
	input.seekg(prg_offset, std::ios::beg);
	input.read((char*)rom->prg.data(), rom->prg.size());

	//	since the chr section is immediately after the prg section, we can read it immediately after
	input.read((char*)rom->chr.data(), rom->chr.size());

	uint8_t mapperID = (rom->header.flags7 & 0b11110000) | ((rom->header.flags6 & 0b11110000) >> 4);
	switch (mapperID) {

	case 0: 
		mapper = new Mapper000(rom);
		break;
	case 1:
		mapper = new Mapper001(rom);
		break;
	default:
		throw CartridgeException("MapperID " + std::to_string(mapperID) + " not supported");

	}

	/*
		after the chr section, there is the possibility of a Miscellaneous section,
		which can be several differe things, depending on the Mapper
		since it is non-vital, we can ignore it for now at least
	*/

	input.close();

	return mapper;

}