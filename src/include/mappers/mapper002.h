#ifndef MAPPER_002_H
#define MAPPER_002_H

#include ".\Mapper.h"

class Mapper002 : public MapperClass {

public:
	Mapper002(romStruct* _rom);
	~Mapper002() { delete rom; }

	[[nodiscard]] uint8_t prg_read(uint16_t& address) override;
	[[nodiscard]] uint8_t chr_read(uint16_t& address) override;

	uint8_t prg_write(uint16_t& address, const uint8_t& data) override;
	uint8_t chr_write(uint16_t& address, const uint8_t& data) override;

protected:
	//	PRG ROM is always 2 16KB windows with the second always fixed to the last section
	uint8_t prg_rom_window_1 = 0x00;
	uint8_t prg_rom_window_2 = 0x00;

};

#endif