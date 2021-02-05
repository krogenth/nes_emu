#include "..\include\mappers\Mapper001.h"

uint8_t Mapper001::prg_read(uint16_t address) {

	return rom->prg.at(address);

}

uint8_t Mapper001::chr_read(uint16_t address) {

	return rom->chr.at(address);

}

void Mapper001::prg_write(uint16_t address, const uint8_t& data) {



}

void Mapper001::chr_write(uint16_t address, const uint8_t& data) {



}