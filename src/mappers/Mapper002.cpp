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
		return 0;
	else if (address < 0xC000)
		return this->rom->prg_rom.at(((size_t)address - 0x8000) + ((size_t)this->prg_rom_window_1 * 0x4000));
	else
		return this->rom->prg_rom.at(((size_t)address - 0xC000) + ((size_t)this->prg_rom_window_2 * 0x4000));
		

}

[[nodiscard]] uint8_t Mapper002::chr_read(uint16_t& address) {

	//	safety check for CPU testing ROMs
	//if (this->rom->chr_rom.size() == 0)
		//return this->rom->chr_ram.at((size_t)address);

	return this->rom->chr_rom.at((size_t)address);

}

uint8_t Mapper002::prg_write(uint16_t& address, const uint8_t& data) {

	if (address < 0x8000)
		return 0;

	//	this follows the UNROM specification, which uses only the first 3 bits to select the PRG ROM section to window
	if(address & 0x8000)
		this->prg_rom_window_1 = data & 0x0F;

	return data;

}

uint8_t Mapper002::chr_write(uint16_t& address, const uint8_t& data) {

	//	safety check for CPU testing ROMs
	//if (this->rom->chr_rom.size() == 0)
		//return this->rom->chr_ram.at((size_t)address) = data;

 	return this->rom->chr_rom.at((size_t)address) = data;

}