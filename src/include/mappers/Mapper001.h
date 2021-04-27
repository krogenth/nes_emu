#ifndef MAPPER_001_H
#define MAPPER_001_H

#include ".\Mapper.h"

class Mapper001 : public MapperClass {

public:
	Mapper001(romStruct* _rom);
	~Mapper001() { delete rom; }

	[[nodiscard]] uint8_t prg_read(uint16_t& address) override;
	[[nodiscard]] uint8_t chr_read(uint16_t& address) override;

	uint8_t prg_write(uint16_t& address, const uint8_t& data) override;
	uint8_t chr_write(uint16_t& address, const uint8_t& data) override;

	mirrorEnum getMirror() { return mirroring; }

protected:
	uint8_t reg_Load = 0b00010000;
	uint8_t reg_Control = 0x00;

	//	PRG ROM can be 256KB or 512KB, but we can only look at 2 16KB sections or 1 32KB section
	//	since 32KB can make 2 16KB, we can treat it as 2 16KB windows always
	uint8_t prg_rom_window_1 = 0x00;
	uint8_t prg_rom_window_2 = 0x00;

	//	PRG RAM is 32KB, but we can only look at 8KB at a time, so we need to have an offset to "look" at a specific section
	uint8_t prg_ram_window = 0x00;

	//	CHR ROM can only be 128KB, and we can only look at 2 4KB sections or 1 8KB section
	uint8_t chr_rom_window_1 = 0x00;
	uint8_t chr_rom_window_2 = 0x00;

	//	used to determine the windowing version
	uint8_t prg_window_mode = 0x00;
	uint8_t chr_window_mode = 0x00;

	//	used to communicate between things, mainly for selecting window
	uint8_t chr_value_1 = 0x00;
	uint8_t chr_value_2 = 0x00;
	uint8_t prg_value_1 = 0x00;

	mirrorEnum mirroring;

	void clearLoad() { this->reg_Load = 0b00010000; }
	void loadControl();
	void updateWindows();

};

#endif
