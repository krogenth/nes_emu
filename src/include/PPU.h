#ifndef PPU_H
#define PPU_H

#include <cinttypes>
#include <tuple>

enum mirrorEnum : uint8_t;

class CartridgeClass;
class GUIClass;

struct Sprite {

	//	see the following for why these are stored in this particular order: https://wiki.nesdev.com/w/index.php/PPU_memory_map
	//	with this, we can efficiently memcpy from the OAMDMA into
	uint8_t y = 0;
	uint8_t index = 0;
	uint8_t attributes = 0;
	uint8_t x = 0;

};

//	flags for stating the bit ordering of registers
enum CTRL_BITMASKS : uint8_t {

	NAMETABLE = 0b00000011,		//	0 = 0x2000, 1 = 0x2400, 2 = 0x2800, 3 = 0x2C00
	INCREMENT = 0b00000100,		//	VRAM increment per CPU read/write of PPUDATA: 0 = +1, 1 = +32
	SPRITE = 0b00001000,		//	Sprite pattern table address, for 8x8 sprites(ignored in 8x16 mode): 0 = 0x0000, 1 = 0x1000
	BACKGROUND = 0b00010000,		//	Background pattern table address: 0 = 0x0000, 1 = 0x1000
	S_HEIGHT = 0b00100000,		//	Sprite size: 0 = 8x8, 1 = 8x16
	MASTER = 0b01000000,		//	PPU Master/Slave selection: 0 = read backdrop from EXT, 1 = write backdrop to EXT
	NMI = 0b10000000			//	Generate NMI at start of vertical blanking: 0 = false, 1 = true

};

enum MASK_BITMASKS : uint8_t {

	GREYSCALE = 0b00000001,
	BG_LEFT_COL = 0b00000010,
	S_LEFT_COL = 0b00000100,
	BG_ENABLE = 0b00001000,
	S_ENABLE = 0b00010000,
	COLOR_EMPH = 0b11100000

};

enum STAT_BITMASKS : uint8_t {

	UNUSED = 0b00011111,
	S_OVERFLOW = 0b00100000,
	S_0_HIT = 0b01000000,
	VBLANK = 0b10000000

};

#pragma pack(1)
struct PPU_Registers {

	//	see following for explaination of PPU registers: https://wiki.nesdev.com/w/index.php/PPU_registers
	uint8_t CTRL = 0;		//	$2000
	uint8_t MASK = 0;		//	$2001
	uint8_t STATUS = 0;		//	$2002
	uint8_t OAMADDR = 0;	//	$2003

	uint8_t OAMDATA = 0;	//	$2004
	uint8_t SCROLL = 0;		//	$2005
	uint8_t ADDR = 0;		//	$2006
	uint8_t DATA = 0;		//	$2007
	uint8_t OAMDMA = 0;		//	$4014

};
#pragma pack()

class PPUClass {

public:
	//	used to access Cartridge or RAM
	uint8_t access(uint16_t& addr, uint8_t& data, bool isWrite);

	//	used to access PPU registers
	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t& data);

	//	used to store components needed to communicate with
	void loadCartridge(CartridgeClass* _cartridge);
	void loadGUI(GUIClass* _GUI);		//	we only need the GUIClass to give SDL2 the new frame to draw

	void cycle();
	void reset();

	std::tuple<uint32_t, uint32_t> getResolution();

	typedef void* (PPUClass::* getPPUData)(void);
	typedef size_t(PPUClass::* getPPUDataSize)(void);

protected:
	//	store the resolution of the image displayed(NTSC, PAL)
	uint32_t tvResolutionX[2] = { 256, 256 };
	uint32_t tvResolutionY[2] = { 240, 288 };
	uint32_t tvFrameRate[2] = { 60, 50 };		//	it does 1/2 frames at Xfps, but full frame is X/2fps

	PPU_Registers registers;

	Sprite OAM[64];

	//	we need to store the cartridge for CHR reading
	CartridgeClass* cartridge = nullptr;
	GUIClass* GUI = nullptr;

	uint16_t addressMirrorAdjust(uint16_t addr);

};

#endif