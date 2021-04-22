#include <tuple>

#include ".\include\PPU.h"
#include ".\include\Cartridge.h"
#include ".\include\romStruct.h"

uint8_t PPUClass::access(uint16_t& addr, uint8_t& data, bool isWrite) {

	//	see the following for the PPU memory layout: https://wiki.nesdev.com/w/index.php/PPU_memory_map
	if (addr < 0x2000)
		return this->cartridge->chr_access(addr, data, isWrite);
	else if (addr < 0x3F00) {

		addr = addr % 0x1000 + 0x2000;
		return 0;

	}
	else
		return 0;

}

uint8_t PPUClass::read(uint16_t addr) {

	//	we need to handle the mirroring, CPU treats 0x2000 to 0x3FFF as PPU space, see: https://wiki.nesdev.com/w/index.php/CPU_memory_map
	addr = (addr % 8) + 0x2000;

	switch (addr) {

	case 0x2000:
		return 0;
	case 0x2001:
		return 0;
	case 0x2002:
		return this->registers.STATUS;
	case 0x2003:
		return 0;
	case 0x2004:
		return this->registers.OAMDATA;
	case 0x2005:
		return 0;
	case 0x2006:
		return this->registers.DATA;
	case 0x2007:
		return this->registers.DATA;
	default:
		return 0;

	}

}

void PPUClass::write(uint16_t addr, uint8_t& data) {

	//	we need to handle the mirroring, CPU treats 0x2000 to 0x3FFF as PPU space, see: https://wiki.nesdev.com/w/index.php/CPU_memory_map
	addr = (addr % 8) + 0x2000;

	switch (addr) {

	case 0x2000:
		this->registers.CTRL = data; break;
	case 0x2001:
		this->registers.MASK = data; break;
	case 0x2002:
		break;
	case 0x2003:
		this->registers.OAMADDR = data; break;
	case 0x2004:
		this->registers.OAMDATA = data; break;
	case 0x2005:
		break;
	case 0x2006:
		this->registers.DATA = data; break;
	case 0x2007:
		this->registers.DATA = data; break;

	}

}

void PPUClass::loadCartridge(CartridgeClass* _cartridge) {

	this->cartridge = _cartridge;

}

void PPUClass::loadGUI(GUIClass* _GUI) {

	this->GUI = _GUI;

}

void PPUClass::cycle() {



}

void PPUClass::reset() {

	//	this currently follows the NTSC version of resetting found here: https://wiki.nesdev.com/w/index.php/PPU_power_up_state

	this->registers.CTRL = 0b00000000;
	this->registers.MASK = 0b00000000;
	this->registers.STATUS = 0b00000000;
	this->registers.SCROLL = 0x00;
	this->registers.DATA = 0x00;

}

std::tuple<uint32_t, uint32_t> PPUClass::getResolution() {

	return std::make_tuple(this->tvResolutionX[this->cartridge->getTV()], this->tvResolutionY[this->cartridge->getTV()]);

}

uint16_t PPUClass::addressMirrorAdjust(uint16_t addr) {

	switch (this->cartridge->getMirror()) {

	case mirrorEnum::HORIZONTAL: return ((addr / 2) & 0x0400) + (addr % 0x0400);
	case mirrorEnum::VERTICAL: return addr % 0x0800;
	default: return addr - 0x2000;

	}

}