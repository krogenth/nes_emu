#ifndef MAPPER_007_H
#define MAPPER_007_H

#include ".\Mapper.h"

class Mapper007 : public MapperClass {

public:
	Mapper007(romStruct* _rom);
	~Mapper007() { delete rom; }

	[[nodiscard]] uint8_t prg_read(uint16_t& address) override;
	[[nodiscard]] uint8_t chr_read(uint16_t& address) override;

	uint8_t prg_write(uint16_t& address, const uint8_t& data) override;
	uint8_t chr_write(uint16_t& address, const uint8_t& data) override;

	mirrorEnum getMirror() { return mirroring; }

protected:
	//	PRG ROM is always 1 32KB window
	uint8_t prg_rom_window_1 = 0x00;

	mirrorEnum mirroring;

};

#endif