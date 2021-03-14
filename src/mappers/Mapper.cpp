#include "..\include\mappers\Mapper.h"

[[nodiscard]] uint8_t MapperClass::prg_read(uint16_t& address) {

	return this->rom->prg_rom.at(address);

}

[[nodiscard]] uint8_t MapperClass::chr_read(uint16_t& address) {

	return this->rom->chr_rom.at(address);

}

uint8_t MapperClass::prg_write(uint16_t& address, const uint8_t& data) {

	return this->rom->prg_rom.at(address) = data;

}

uint8_t MapperClass::chr_write(uint16_t& address, const uint8_t& data) {

	return 0;

}