#ifndef MAPPER_H
#define MAPPER_H

#include <cinttypes>	//	(u)intx_t datatypes

#include "..\RomStruct.h"

class Mapper {

public:
	Mapper(romStruct* _rom) { rom = _rom; }
	~Mapper() { delete rom; }

	virtual uint8_t prg_read(uint16_t address) = 0;
	virtual uint8_t chr_read(uint16_t address) = 0;

	virtual void prg_write(uint16_t address, const uint8_t& data) = 0;
	virtual void chr_write(uint16_t address, const uint8_t& data) = 0;

protected:
	romStruct* rom = nullptr;

};

#endif