#ifndef ROM_H
#define ROM_H

#include <cinttypes>	//	(u)intx_t datatypes
#include <string>		//	std::string
#include <vector>

static const std::string nesMagic = { 'N', 'E', 'S', (char)0x1A };

struct romHeaderStruct {

	char ID[4] = { ' ', ' ', ' ', ' ' };
	uint8_t prgRomSizeLSB = 0;
	uint8_t chrRomSizeLSB = 0;
	uint8_t flags6 = 0;						//	mirroring(0), battery(1), trainer(2)(512 bytes, after header), four-screen(3), mapperNumLow(4-7)
	uint8_t flags7 = 0;						//	console(0-1), NES2ID(2-3), mapperNumHi(4-7)
	uint8_t flags8 = 0;						//	mapperNum(0-3), submapperNum(4-7)
	uint8_t prgChrSizeMSB = 0;				//	prgRomSizeMSB(0-3), chrRomSizeMSB(4-7)
	uint8_t prgRamSize = 0;					//	prgRamSizeVolatileShift(0-3), prgRamSizeNonVolatileShift(4-7), if non-0, size is 64 << size
	uint8_t chrRamSize = 0;					//	chrRamSizeVolatileShift(0-3), chrRamSizeNonVolatileShift(4-7), if non-0, size is 64 << size
	uint8_t timing = 0;						//	CPU/PPU_Time(0-1), unused(2-7)
	uint8_t flags13 = 0;					//	PPUType(0-3), hardwareType(4-7), when flags7 & 3 == 1(Vs. System) :: extendedConsoleType(0-3), when flags7 & 3 == 3(extended console type)
	uint8_t miscROMs = 0;					//	miscROMs(0-1), unused(2-7)
	uint8_t expandDevice = 0;				//	expansionDevice(0-5), unused(6-7)

};

struct romStruct {

	romHeaderStruct header;
	std::vector<uint8_t> prg;
	std::vector<uint8_t> chr;
	uint32_t prg_chunks = 0;
	uint32_t chr_chunks = 0;

};

enum mirrorEnum {

	HORIZONTAL = 0,
	VERTICAL = 1

};

#endif