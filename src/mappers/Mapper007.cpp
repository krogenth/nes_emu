#include "..\include\mappers\Mapper007.h"

#include "..\include\custom_exceptions.h"

Mapper007::Mapper007(romStruct* _rom) : MapperClass(_rom) {

	this->mirroring = mirrorEnum::SINGLEA;

}

[[nodiscard]] uint8_t Mapper007::prg_read(uint16_t& address) {

	if (address < 0x8000)
		throw new MapperException("Mapper does not support PRG RAM");
	else
		return this->rom->prg_rom.at(((size_t)address - 0x8000) + (this->prg_rom_window_1 * 0x8000));

}

[[nodiscard]] uint8_t Mapper007::chr_read(uint16_t& address) {

	return this->rom->chr_rom.at((size_t)address);

}

uint8_t Mapper007::prg_write(uint16_t& address, const uint8_t& data) {

/*
7  bit  0
---- ----
xxxM xPPP
   |  |||
   |  +++- Select 32 KB PRG ROM bank for CPU $8000-$FFFF
   +------ Select 1 KB VRAM page for all 4 nametables
*/

	if (address < 0x8000)
		throw new MapperException("Mapper does not support PRG RAM");

	if (!(data & 0b00010000))
		this->mirroring = mirrorEnum::SINGLEA;
	else
		this->mirroring = mirrorEnum::SINGLEB;

	return this->prg_rom_window_1 = data & 0x07;

}

uint8_t Mapper007::chr_write(uint16_t& address, const uint8_t& data) {

	return this->rom->chr_rom.at((size_t)address) = data;

}