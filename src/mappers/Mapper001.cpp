#include "..\include\mappers\Mapper001.h"

[[nodiscard]] uint8_t Mapper001::prg_read(uint16_t& address) {

	return rom->prg_rom.at(address);

}

[[nodiscard]] uint8_t Mapper001::chr_read(uint16_t& address) {

	return rom->chr_rom.at(address);

}

uint8_t Mapper001::prg_write(uint16_t& address, const uint8_t& data) {

	return 0;

}

uint8_t Mapper001::chr_write(uint16_t& address, const uint8_t& data) {

	return 0;

}