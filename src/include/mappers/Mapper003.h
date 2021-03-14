#ifndef MAPPER_003_H
#define MAPPER_003_H

#include ".\Mapper.h"

class Mapper003 : public MapperClass {

public:
	Mapper003(romStruct* _rom);
	~Mapper003() { delete rom; }

	[[nodiscard]] uint8_t prg_read(uint16_t& address) override;
	[[nodiscard]] uint8_t chr_read(uint16_t& address) override;

	uint8_t prg_write(uint16_t& address, const uint8_t& data) override;
	uint8_t chr_write(uint16_t& address, const uint8_t& data) override;

protected:
	//	CHR ROM is always 1 8KB window
	uint8_t chr_rom_window_1 = 0x00;

};

#endif