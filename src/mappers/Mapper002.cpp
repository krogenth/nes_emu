#include "..\include\mappers\Mapper002.h"

#include "..\include\custom_exceptions.h"

//	please see the following for the technical workings: https://wiki.nesdev.com/w/index.php/UxROM
//	only NES 2.0 can fully support the 4MB version, therefore we can safely ignore it, at least for now

Mapper002::Mapper002(romStruct* _rom) : MapperClass(_rom) {

	//	second window is always set to the last bank
	this->prg_rom_window_2 = this->rom->prg_rom_chunks - 1;

}

[[nodiscard]] uint8_t Mapper002::prg_read(uint16_t& address) {

	if (address < 0x8000)
		throw new MapperException("Mapper does not support PRG RAM");
	else if (address < 0xC000)
		return this->rom->prg_rom.at(((size_t)address - 0x8000) + ((size_t)this->prg_rom_window_1 * 0x4000));
	else
		return this->rom->prg_rom.at(((size_t)address - 0xC000) + ((size_t)this->prg_rom_window_2 * 0x4000));
		

}

[[nodiscard]] uint8_t Mapper002::chr_read(uint16_t& address) {

	//	safety check for CPU testing ROMs
	if (this->rom->chr_rom.size() == 0)
		return 0;

	return this->rom->chr_rom.at((size_t)address & 0x1FFF);

}

uint8_t Mapper002::prg_write(uint16_t& address, const uint8_t& data) {

	if (address < 0x8000)
		throw new MapperException("Mapper does not support PRG RAM");

	//	this follows the UNROM specification, which uses only the first 3 bits to select the PRG ROM section to window
	return this->prg_rom_window_1 = data & 0x07;

}

uint8_t Mapper002::chr_write(uint16_t& address, const uint8_t& data) {

	//	safety check for CPU testing ROMs
	if (this->rom->chr_rom.size() == 0)
		return 0;

	return this->rom->chr_rom.at((size_t)address & 0x1FFF) = data;

}