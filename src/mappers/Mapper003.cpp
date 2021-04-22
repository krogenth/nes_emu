#include "..\include\mappers\Mapper003.h"

#include "..\include\custom_exceptions.h"

Mapper003::Mapper003(romStruct* _rom) : MapperClass(_rom) {



}

[[nodiscard]] uint8_t Mapper003::prg_read(uint16_t& address) {

	if (address < 0x8000)
		throw new MapperException("Mapper does not support PRG RAM");
	else
		return this->rom->prg_rom.at(((size_t)address - 0x8000) & ((this->rom->prg_rom_chunks > 1) ? 0x7FFF : 0x3FFF));

}

[[nodiscard]] uint8_t Mapper003::chr_read(uint16_t& address) {

	//	safety check for CPU testing ROMs
	if (this->rom->chr_rom.size() == 0)
		return 0;

	return this->rom->chr_rom.at((size_t)address + ((size_t)this->chr_rom_window_1 * 0x2000));

}

uint8_t Mapper003::prg_write(uint16_t& address, const uint8_t& data) {

	if(address < 0x8000)
		throw new MapperException("Mapper does not support PRG RAM");
	
	//	follows CNROM format for selecting correct bank to window to
	return this->chr_rom_window_1 = data & 0x03;

}

uint8_t Mapper003::chr_write(uint16_t& address, const uint8_t& data) {

	//	safety check for CPU testing ROMs
	if (this->rom->chr_rom.size() == 0)
		return 0;

	return this->rom->chr_rom.at((size_t)address + ((size_t)this->chr_rom_window_1 * 0x2000)) = data;

}