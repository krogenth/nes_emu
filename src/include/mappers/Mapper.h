#ifndef MAPPER_H
#define MAPPER_H

#include <cinttypes>	//	(u)intx_t datatypes

#include "..\RomStruct.h"

class MapperClass {

public:
	MapperClass(romStruct* _rom) { rom = _rom; }
	~MapperClass() { this->reset(); }

	virtual void reset() { delete rom; };

	virtual uint8_t prg_read(uint16_t address);
	virtual uint8_t chr_read(uint16_t address);

	virtual void prg_write(uint16_t address, const uint8_t& data);
	virtual void chr_write(uint16_t address, const uint8_t& data);

protected:
	romStruct* rom = nullptr;

};

#endif