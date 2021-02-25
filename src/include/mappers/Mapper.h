#ifndef MAPPER_H
#define MAPPER_H

#include <cinttypes>	//	(u)intx_t datatypes

#include "..\RomStruct.h"

class MapperClass {

public:
	MapperClass(romStruct* _rom) { rom = _rom; }
	~MapperClass() { delete rom; }

	//	we do not want these values thrown away, therefore we must demand the value be used
	[[nodiscard]] virtual uint8_t prg_read(uint16_t& address);
	[[nodiscard]] virtual uint8_t chr_read(uint16_t& address);

	virtual uint8_t prg_write(uint16_t& address, const uint8_t& data);
	virtual uint8_t chr_write(uint16_t& address, const uint8_t& data);

	mirrorEnum getMirror() { return (rom->header.flags6 & 0x01) ? mirrorEnum::VERTICAL : mirrorEnum::HORIZONTAL; }
	tvEnum getTV() { return rom->tvType; }

protected:
	romStruct* rom = nullptr;

};

#endif