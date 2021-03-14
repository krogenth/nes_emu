#ifndef MAPPER_H
#define MAPPER_H

#include <cinttypes>	//	(u)intx_t datatypes

#include "..\RomStruct.h"

class MapperClass {

public:
	MapperClass(romStruct* _rom) { this->rom = _rom; }
	~MapperClass() { delete this->rom; }

	//	we do not want these values thrown away, therefore we must demand the value be used
	[[nodiscard]] virtual uint8_t prg_read(uint16_t& address);
	[[nodiscard]] virtual uint8_t chr_read(uint16_t& address);

	virtual uint8_t prg_write(uint16_t& address, const uint8_t& data);
	virtual uint8_t chr_write(uint16_t& address, const uint8_t& data);

	virtual mirrorEnum getMirror() { return (this->rom->header.flags6 & 0x01) ? mirrorEnum::VERTICAL : mirrorEnum::HORIZONTAL; }
	tvEnum getTV() { return this->rom->tvType; }

	//	we do not need to test if the ROM exists, because if the mapper exists, the ROM does too
	uint8_t* get_prm_rom() { return &this->rom->prg_rom.at(0); }
	size_t get_prm_rom_size() { return this->rom->prg_rom.size(); }

	uint8_t* get_prg_ram() { return (this->rom->prg_ram.size()) ? &this->rom->prg_ram.at(0) : 0; }
	size_t get_prg_ram_size() { return this->rom->prg_ram.size(); }

	uint8_t* get_chr_rom() { return (this->rom->chr_rom.size()) ? &this->rom->chr_rom.at(0) : 0; }
	size_t get_chr_rom_size() { return this->rom->chr_rom.size(); }

protected:
	romStruct* rom = nullptr;

};

#endif