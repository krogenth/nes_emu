#include "..\include\mappers\Mapper.h"

uint8_t MapperClass::prg_read(uint16_t& address) {

	return rom->prg_rom.at(address);

}

uint8_t MapperClass::chr_read(uint16_t& address) {

	return rom->chr_rom.at(address);

}

uint8_t MapperClass::prg_write(uint16_t& address, const uint8_t& data) {

	return rom->prg_rom.at(address) = data;

}

uint8_t MapperClass::chr_write(uint16_t& address, const uint8_t& data) {

	return rom->chr_rom.at(address) = data;

}