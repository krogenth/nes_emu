#ifndef PPU_H
#define PPU_H

#include <cinttypes>

enum mirrorEnum;

class CartridgeClass;

struct Sprite {

	//	see the following for why these are stored in this particular order: https://wiki.nesdev.com/w/index.php/PPU_memory_map
	//	with this, we can efficiently memcpy from the OAMDMA into
	uint8_t y = 0;
	uint8_t index = 0;
	uint8_t attributes = 0;
	uint8_t x = 0;

};

class PPUClass {

public:
	//	used to access Cartridge opr RAM
	uint8_t access(uint16_t& addr, uint8_t& data, bool isWrite);

	//	used to access PPU registers
	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t& data);

	void loadCartridge(CartridgeClass* _cartridge);
	void cycle();
	void reset();

protected:
	//	see following for explaination of PPU registers: https://wiki.nesdev.com/w/index.php/PPU_registers
	uint8_t PPUCTRL = 0;		//	$2000
	uint8_t PPUMASK = 0;		//	$2001
	uint8_t PPUSTATUS = 0;		//	$2002
	uint8_t OAMADDR = 0;		//	$2003
	uint8_t OAMDATA = 0;		//	$2004
	uint8_t PPUSCROLL = 0;		//	$2005
	uint8_t PPUADDR = 0;		//	$2006
	uint8_t PPUDATA = 0;		//	$2007
	uint8_t OAMDMA = 0;			//	$4014


	Sprite OAM[64];

	//	store the mirroring
	mirrorEnum mirroring;

	//	we need to store the cartridge for CHR reading
	CartridgeClass* cartridge;

	//	used to set the mirroring
	void setMirror(mirrorEnum);
	uint16_t addressMirrorAdjust(uint16_t addr);

};

#endif