#include "..\include\mappers\Mapper.h"

uint8_t MapperClass::prg_read(uint16_t address) {

	return rom->prg.at(address);

}

uint8_t MapperClass::chr_read(uint16_t address) {

	return rom->chr.at(address);

}

void MapperClass::prg_write(uint16_t address, const uint8_t& data) {



}

void MapperClass::chr_write(uint16_t address, const uint8_t& data) {



}