#include <fstream>	//	std::ifstream
#include <filesystem>

#include ".\include\Cartridge.h"
#include ".\include\Mapper_Collection.h"

#include ".\include\io_util.h"
#include ".\include\custom_exceptions.h"

[[nodiscard]] romStruct* CartridgeClass::load(std::string filename) {

	//	following the nesdev.com documentation on the NES 2.0 header: https://wiki.nesdev.com/w/index.php/NES_2.0

	std::ifstream input(filename, std::ios::in | std::ios::binary);
	if (!input.is_open())
		throw CartridgeException("Cartridge file: " + filename + " not found");

	romStruct* rom = new romStruct;
	rom->header = readRaw<romHeaderStruct>(input);

	//	ensure the file read is an NES rom by checking the ID
	//	if not, close file and delete pointer to remove memory leaks
	if (nesMagic.compare(0, 4, rom->header.ID, 4) != 0) {

		input.close();
		delete rom;
		throw CartridgeException("Cartridge not NES format. HeaderID: " + std::string(rom->header.ID));	

	}

	//	we need to unload the current cartridge to replace it with the new one
	this->unload();

	/*
	to check the mirroring the NES rom uses, check:
		rom->header.flags6 & 0b00000001
		0 will be horizontal mirroring, 1 will be vertical mirroring
	*/

	//	check if this is an NES 2.0 rom: https://wiki.nesdev.com/w/index.php/NES_2.0
	//	this is according to the nesdev documentation, the bits should be 10 to be classified as NES 2.0
	if ((rom->header.flags7 & 0b00001100) == 0b00001000) {

		//	we want to store the number of chunks each segment has as well for later use with the Mapper
		rom->prg_rom_chunks = (uint32_t)rom->header.prgRomSizeLSB | ((uint32_t)(rom->header.prgChrSizeMSB & 0b00001111) << 8);
		rom->chr_rom_chunks = (uint32_t)rom->header.chrRomSizeLSB | ((uint32_t)(rom->header.prgChrSizeMSB & 0b11110000) << 4);
		rom->prg_rom.resize((size_t)rom->prg_rom_chunks * 0x4000);		//	0x4000 == 16KB
		rom->chr_rom.resize((size_t)rom->chr_rom_chunks * 0x2000);		//	0x2000 == 8KB
		(rom->header.timing & 0x03) ? rom->tvType = tvEnum::PAL : rom->tvType = tvEnum::NTSC;
		
		/*
		NES 2.0 includes specifics for PRG RAM
		*/

	}
	//	otherwise it's an iNES rom: https://wiki.nesdev.com/w/index.php/INES
	else {

		rom->prg_rom_chunks = (uint32_t)rom->header.prgRomSizeLSB;
		rom->chr_rom_chunks = rom->header.chrRomSizeLSB ? rom->header.chrRomSizeLSB : 1;
		rom->prg_ram_chunks = rom->header.flags8 ? rom->header.flags8 : 1;							//	there will always be minimum one 8KB chunk of PRG RAM
		rom->prg_rom.resize((size_t)rom->prg_rom_chunks * 0x4000);									//	0x4000 == 16KB
		rom->chr_rom.resize((size_t)rom->chr_rom_chunks * 0x2000);									//	0x2000 == 8KB
		rom->prg_ram.resize((size_t)rom->prg_ram_chunks * 0x2000);
		(rom->header.prgChrSizeMSB & 0x01) ? rom->tvType = tvEnum::PAL : rom->tvType = tvEnum::NTSC;

	}

	//	read in prg and chr data into their respective vectors
	//	for the prg section, we need to ignore the header(16 bytes) and the possible "trainer" section(512 bytes)
	uint32_t prg_offset = 16 + (512 * ((rom->header.flags6 & 0b00000100) >> 2));
	input.seekg(prg_offset, std::ios::beg);
	input.read((char*)rom->prg_rom.data(), rom->prg_rom.size());

	//	since the chr section is immediately after the prg section, we can read it immediately after
	input.read((char*)rom->chr_rom.data(), rom->chr_rom.size());

	/*
		after the chr section, there is the possibility of a Miscellaneous section,
		which can be several different things, depending on the Mapper
		since it is non-vital, we can ignore it for now at least
	*/

	input.close();

	//	check if a save file exists
	if (!std::filesystem::exists(".\\saves"))
		std::filesystem::create_directory(".\\saves");
	this->saveFile = ".\\saves\\" + filename.substr(filename.rfind("\\") + 1, filename.find(".nes") - (filename.rfind("\\") + 1)) + ".prgram";
	if (std::filesystem::exists(this->saveFile)) {

		input.open(this->saveFile, std::ios::in | std::ios::binary);
		input.read((char*)rom->prg_ram.data(), rom->prg_ram.size());
		input.close();

	}

	return rom;

}

void CartridgeClass::unload() {

	if (this->mapper != nullptr) {

		delete this->mapper;
		this->mapper = nullptr;

	}

}

bool CartridgeClass::loaded() {

	return (this->mapper != nullptr);

}

void CartridgeClass::storeMapper(MapperClass* _mapper) {

	if (this->mapper != nullptr)
		delete this->mapper;
	
	this->mapper = _mapper;

}

void CartridgeClass::reset() {
	
	//	needs to set data to a fixed state the CPU expects, see http://wiki.nesdev.com/w/index.php/CPU_memory_map

}

uint8_t CartridgeClass::prg_access(uint16_t& address, const uint8_t& data, bool isWrite) {

	if (isWrite == true)
		return this->mapper->prg_write(address, data);
	
	return this->mapper->prg_read(address);

}

uint8_t CartridgeClass::chr_access(uint16_t& address, const uint8_t& data, bool isWrite) {

	if (isWrite == true)
		return this->mapper->chr_write(address, data);
	
	return this->mapper->chr_read(address);

}

mirrorEnum CartridgeClass::getMirror() {
	
	if (this->mapper != nullptr)
		return this->mapper->getMirror();
	throw new CartridgeException("Mapper not found");

}

tvEnum CartridgeClass::getTV() {

	if (this->mapper != nullptr)
		return this->mapper->getTV();
	throw new CartridgeException("Mapper not found");

}

void* CartridgeClass::get_prm_rom() {
	
	if (this->mapper != nullptr)
		return (void*)this->mapper->get_prm_rom();
	return 0;

}

size_t CartridgeClass::get_prm_rom_size() {
	
	if (this->mapper != nullptr)
		return this->mapper->get_prm_rom_size();
	return 0;

}

void* CartridgeClass::get_prg_ram() {

	if (this->mapper != nullptr)
		return (void*)this->mapper->get_prg_ram();
	return 0;

}
size_t CartridgeClass::get_prg_ram_size() {
	
	if (this->mapper != nullptr)
		return this->mapper->get_prg_ram_size();
	return 0;

}

void* CartridgeClass::get_chr_rom() {
	
	if (this->mapper != nullptr)
		return (void*)this->mapper->get_chr_rom();
	return 0;

}

size_t CartridgeClass::get_chr_rom_size() {
	
	if (this->mapper != nullptr)
		return this->mapper->get_chr_rom_size();
	return 0;

}