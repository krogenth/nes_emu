#include "..\include\mappers\Mapper000.h"

uint8_t Mapper000::prg_read(uint16_t address) {

	return rom->prg.at(address);

}

uint8_t Mapper000::chr_read(uint16_t address) {

	return rom->chr.at(address);

}

void Mapper000::prg_write(uint16_t address, const uint8_t& data) {

	

}

void Mapper000::chr_write(uint16_t address, const uint8_t& data) {

	

}