#ifndef PPU_H
#define PPU_H

#include <cinttypes>
#include <tuple>
#include <vector>

#include ".\SFML\Graphics.hpp"

enum mirrorEnum : uint8_t;

class CartridgeClass;
class CPUClass;
class GUIClass;

struct Sprite {

	//	see the following for why these are stored in this particular order: https://wiki.nesdev.com/w/index.php/PPU_memory_map
	//	with this, we can efficiently memcpy from the OAMDMA into
	uint8_t y = 0;
	uint8_t index = 0;
	uint8_t attributes = 0;
	uint8_t x = 0;

};

struct SpriteContainer {

	uint8_t id = 0;
	Sprite data;
	uint8_t dataH = 0;
	uint8_t dataL = 0;

};

//	Loopy's VRAM addressing
union Address
{
	struct
	{
		unsigned cX : 5;  // Coarse X.
		unsigned cY : 5;  // Coarse Y.
		unsigned nt : 2;  // Nametable.
		unsigned fY : 3;  // Fine Y.
	};
	struct
	{
		unsigned l : 8;
		unsigned h : 7;
	};
	unsigned addr : 14;
	unsigned r : 15;
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

enum SCANLINE_TYPES : uint8_t {

	//	in order to properly handle what happens when in the scanlines, please follow this: https://wiki.nesdev.com/w/images/d/d1/Ntsc_timing.png

	//	scanline timings are given in terms of NTSC, NOT PAL
	PRE = 0b00000001,			//	(-1/261)	this is what happens before the visible frame is started, it is only 1 scanline: https://wiki.nesdev.com/w/index.php/PPU_rendering#Pre-render_scanline_.28-1_or_261.29
	FRAME = 0b00000010,			//	(0-239)		this is the frame that is draw to the window: https://wiki.nesdev.com/w/index.php/PPU_rendering#Visible_scanlines_.280-239.29
	POST = 0b00000100,			//	(240)		this is the scanline after the visible frame is handled: https://wiki.nesdev.com/w/index.php/PPU_rendering#Post-render_scanline_.28240.29
	BLANK = 0b00001000			//	(240-260)	this is the vertical blanking that happens after the post scanline: https://wiki.nesdev.com/w/index.php/PPU_rendering#Post-render_scanline_.28240.29

};

#pragma pack(1)
struct PPU_Registers {

	//	see following for explaination of PPU registers: https://wiki.nesdev.com/w/index.php/PPU_registers
	uint8_t CTRL = 0;		//	$2000
	uint8_t MASK = 0;		//	$2001
	uint8_t STAT = 0;		//	$2002
	uint8_t OAMADDR = 0;	//	$2003

	//uint8_t OAMDATA = 0;	//	$2004		//	this register is effectively useless, we can address the OAM memory directly instead
	//uint8_t SCROLL = 0;	//	$2005		//	this register is effectively useless, because we can use Loopy's
	//uint8_t ADDR = 0;		//	$2006		//	this register is effectively useless, because we can use Loopy's
	//uint8_t DATA = 0;		//	$2007		//	this register is effectively useless, because we can just store the data immediately instead
	//uint8_t OAMDMA = 0;	//	$4014		//	also effectively useless, it only matters if the CPU writes to the address of $4014, not what it writes

};
#pragma pack()

class PPUClass {

public:
	PPUClass();
	~PPUClass() {  }

	//	used to access PPU registers
	void write(uint16_t addr, uint8_t& data);
	uint8_t read(uint16_t addr);

	//	used to store components needed to communicate with
	void loadCartridge(CartridgeClass* _cartridge) { this->cartridge = _cartridge; }
	void loadCPU(CPUClass* _CPU) { this->CPU = _CPU; }
	void loadGUI(GUIClass* _GUI) { this->GUI = _GUI; }		//	we only need the GUIClass to give SDL2 the new frame to draw

	std::tuple<uint32_t, uint32_t> getResolution();
	uint32_t getTVFrameRate();

	void reset();
	void cycle();

	void* get_ppu_regs() { return (void*)&this->registers; }
	size_t get_ppu_regs_size() { return sizeof(this->registers); }

	void* get_ppu_nametables() { return (void*)&this->nametables.at(0); }
	size_t get_ppu_nametables_size() { return this->nametables.size(); }

	void* get_ppu_palettes() { return (void*)&this->palettes.at(0); }
	size_t get_ppu_palettes_size() { return this->palettes.size(); }

	void* get_ppu_oam() { return (void*)(&this->OAM); }
	size_t get_ppu_oam_size() { return 0x0100; }

	void* get_ppu_primary_oam() { return (void*)(&this->primaryOAM); }
	void* get_ppu_secondary_oam() { return (void*)(&this->secondaryOAM); }
	size_t get_ppu_oam_reg_size() { return (sizeof(SpriteContainer) * 8); }

	//	function pointers for GUI access to CPU data
	typedef void* (PPUClass::* getPPUData)(void);
	typedef size_t(PPUClass::* getPPUDataSize)(void);

	typedef void* (PPUClass::* getPPUData)(void);
	typedef size_t(PPUClass::* getPPUDataSize)(void);

protected:
	//	registers within the PPU
	PPU_Registers registers;

	//	this is used for drawing to the screen, every time a ROM is loaded, we must change the size of this
	sf::Image pixels;

	//	used to determine what scanline we are currently on
	int16_t scanline = -1;
	int16_t scanlinePixel = 0;

	std::vector<uint8_t> nametables;

	//	the actual palette storage for the PPU
	std::vector<uint8_t> palettes;

	//	this follows directly with RGBA32(NOT ARGB32) which SFML uses
	uint32_t paletteColors[64] = { 0x666666FF, 0x002A88FF, 0x1412A7FF, 0x3B00A4FF, 0x5C007EFF, 0x6E0040FF, 0x6C0600FF, 0x561D00FF,
									0x333500FF, 0x0B4800FF, 0x005200FF, 0x004F08FF, 0x00404DFF, 0x000000FF, 0x000000FF, 0x000000FF,
									0xADADADFF, 0x155FD9FF, 0x4240FFFF, 0x7527FEFF, 0xA01ACCFF, 0xB71E7BFF, 0xB53120FF, 0x994E00FF,
									0x6b6d00FF, 0x388700FF, 0x0c9300FF, 0x008F32FF, 0x007C8DFF, 0x000000FF, 0x000000FF, 0x000000FF,
									0xFFFEFFFF, 0x64B0FFFF, 0x9290FFFF, 0xC676FFFF, 0xF36AFFFF, 0xFE6ECCFF, 0xFE8170FF, 0xEA9E22FF,
									0xBCBE00FF, 0x88D800FF, 0x5CE430FF, 0x45E082FF, 0x48CDDEFF, 0x4F4F4FFF, 0x000000FF, 0x000000FF,
									0xFFFEFFFF, 0xC0DFFFFF, 0xd3D2FFFF, 0xE8C8FFFF, 0xFBC2FFFF, 0xFEC4EAFF, 0xFECCC5FF, 0xF7D8A5FF,
									0xE4E594FF, 0xCFEF96FF, 0xBDf4ABFF, 0xB3F3CCFF, 0xB5EBF2FF, 0xB8B8B8FF, 0x000000FF, 0x000000FF };

	//	for OAM data, please see: https://wiki.nesdev.com/w/index.php/PPU_OAM#Internal_operation
	//	in short, the secondary is filled by the OAM, which in turn initializes 8 internal sprite outputs
	std::vector<uint8_t> OAM;
	SpriteContainer primaryOAM[8];
	SpriteContainer secondaryOAM[8];

	//	Loopy's V and T addresses
	Address vAddr, tAddr;

	//	latches
	uint8_t NT = 0, AT = 0, BGL = 0, BGH = 0;
	bool ATLatchL = false, ATLatchH = false;

	//	shift registers
	uint16_t atShiftL = 0, atShiftH = 0, bgShiftL = 0, bgShiftH = 0;

	//	latch for determining if has been an register access already
	bool latch = false;

	//	
	uint8_t result = 0;

	//	Loopy's fineX
	uint8_t fineX = 0;

	bool isOddFrame = false;

	//	we need to store the cartridge for CHR reading
	CartridgeClass* cartridge = nullptr;
	CPUClass* CPU = nullptr;
	GUIClass* GUI = nullptr;

	//	store the resolution of the image displayed(NTSC, PAL)
	uint32_t tvResolutionX[2] = { 256, 256 };
	uint32_t tvResolutionY[2] = { 240, 288 };
	uint32_t tvFrameRate[2] = { 60, 50 };		//	it does 1/2 frames at Xfps, but full frame is X/2fps

	//	used to access Cartridge or RAM
	uint8_t access(uint16_t addr, uint8_t data = 0, bool isWrite = false);

	void pre_scanline();
	void frame_scanline();
	void post_scanline();
	void blank_scanline();

	bool rendering() { return ((this->registers.MASK & MASK_BITMASKS::BG_ENABLE) || (this->registers.MASK & MASK_BITMASKS::S_ENABLE)); }
	uint32_t sprite_height() { return (this->registers.CTRL & CTRL_BITMASKS::S_HEIGHT) ? 16 : 8; }

	uint16_t addressNT() { return (0x2000 | (this->vAddr.r & 0x0FFF)); }
	uint16_t addressAT() { return (0x23C0 | (this->vAddr.nt << 10) | ((this->vAddr.cY / 4) << 3) | (this->vAddr.cX / 4)); }
	uint16_t addressBG() { return (((this->registers.CTRL & CTRL_BITMASKS::BACKGROUND) * 0x1000) + (this->NT * 16) + this->vAddr.fY); }

	void horizontal_scroll();
	void vertical_scroll();

	void horizontal_update() { if (!this->rendering()) return; this->vAddr.r = ((this->vAddr.r & ~0x041F) | (this->tAddr.r & 0x041F)); }
	void vertical_update() { if (!this->rendering()) return; this->vAddr.r = ((this->vAddr.r & ~0x7BE0) | (this->tAddr.r & 0x7BE0)); }

	void reload_shift();

	void clear_oam();
	void evaluate_sprites();
	void load_sprites();

	void pixelDraw();

	uint16_t addressMirrorAdjust(uint16_t addr);

};

#endif