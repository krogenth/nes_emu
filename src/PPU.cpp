#include <tuple>
#include <iostream>
#include <algorithm>

#include ".\include\PPU.h"
#include ".\include\CPU.h"
#include ".\include\GUI.h"
#include ".\include\Cartridge.h"
#include ".\include\romStruct.h"

#include ".\include\custom_exceptions.h"

PPUClass::PPUClass() {

	//	by documentation, default nametable space is only 2kb of VRAM for nametables: https://wiki.nesdev.com/w/index.php/PPU_memory_map
	this->nametables.resize(0x0800);
	this->palettes.resize(0x0020);
	this->OAM.resize(0x0100);
	
}

uint8_t PPUClass::access(uint16_t addr, uint8_t data, bool isWrite) {

	//	see the following for the PPU memory layout: https://wiki.nesdev.com/w/index.php/PPU_memory_map
	if (addr < 0x2000) // CHR-ROM or CHR-RAM
		return this->cartridge->chr_access(addr, data, isWrite);
	else if (addr <= 0x3EFF) { // Nametable memory
		
		if (isWrite) {

			this->nametables[this->addressMirrorAdjust(addr)] = data;
			return 0;

		}

		return this->nametables[this->addressMirrorAdjust(addr)];

	}
	else if (addr <= 0x3FFF) { // Palette Memory

		if (isWrite) {

			if ((addr & 0x13) == 0x10) addr &= ~0x10;
			this->palettes.at(addr & 0x1F) = data;
			return 0;

		}
		else {

			if ((addr & 0x13) == 0x10) addr &= ~0x10;
			return (this->palettes.at(addr & 0x1F) & ((this->registers.MASK & MASK_BITMASKS::GREYSCALE) ? 0x30 : 0xFF));

		}

	}
	else
		return 0;

}

void PPUClass::write(uint16_t addr, uint8_t& data) {

	//	we need to handle the mirroring, CPU treats 0x2000 to 0x3FFF as PPU space, see: https://wiki.nesdev.com/w/index.php/CPU_memory_map
	addr = (addr % 8);
	this->result = data;

	switch (addr) {

	case 0x0000: // CTRL Register
		this->registers.CTRL = data;
		this->tAddr.nt = this->registers.CTRL & CTRL_BITMASKS::NAMETABLE;
		break;

	case 0x0001: // MASK Register
		this->registers.MASK = data;
		break;

	case 0x0003: // OAMADDR Register
		this->registers.OAMADDR = data;
		break;

	case 0x0004: // OAMDATA Register
		this->OAM[this->registers.OAMADDR++] = data;
		break;

	case 0x0005: // PPUSCROLL Register
		if (!this->latch) {

			this->fineX = data & 7;
			this->tAddr.cX = data >> 3;

		}
		else {

			this->tAddr.fY = data & 7;
			this->tAddr.cY = data >> 3;

		}
		this->latch = !this->latch;
		break;

	case 0x0006: // PPUADDR Register
		if (!this->latch)
			this->tAddr.h = data & 0x3F;
		else {

			this->tAddr.l = data;
			this->vAddr.r = this->tAddr.r;

		}
		this->latch = !this->latch;
		break;

	case 0x0007: // PPUDATA Register
		this->access(this->vAddr.addr, data, true);
		this->vAddr.addr += (this->registers.CTRL & CTRL_BITMASKS::INCREMENT) ? 32 : 1;
		break;

	}

}

uint8_t PPUClass::read(uint16_t addr) {

	//	we need to handle the mirroring, CPU treats 0x2000 to 0x3FFF as PPU space, see: https://wiki.nesdev.com/w/index.php/CPU_memory_map
	addr = (addr % 8);
	static uint8_t buffer = 0;

	switch (addr) {

	case 0x0002: // STATUS Register
		this->result = (this->registers.STAT & 0xE0) | (this->result & 0x1F);

		ClearRegisterBits(this->registers.STAT, STAT_BITMASKS::VBLANK);
		this->latch = false;
		return this->result;

	case 0x0004: // OAMDATA Register
		return this->result = this->OAM[this->registers.OAMADDR];

	case 0x0007: // PPUDATA Register
		this->result = buffer;
		buffer = this->access(this->vAddr.addr);
		if (this->vAddr.addr >= 0x3F00)
			this->result = buffer;
		this->vAddr.addr += (this->registers.CTRL & CTRL_BITMASKS::INCREMENT) ? 32 : 1;
		return this->result;

	default:
		return this->result;

	}

}

std::tuple<uint32_t, uint32_t> PPUClass::getResolution() {

	try {

		return std::make_tuple(this->tvResolutionX[this->cartridge->getTV()], this->tvResolutionY[this->cartridge->getTV()]);

	}
	catch (const MapperException& e) {

		// Can be replaced with error logging once merged.
		std::cout << e.what() << '\n';		//	placeholder until a better solution is created, we should really use a log file instead
		return std::make_tuple(0, 0);

	}

}

uint32_t PPUClass::getTVFrameRate() {

	try {

		return this->tvFrameRate[this->cartridge->getTV()];

	}
	catch (const MapperException& e) {

		// Can be replaced with error logging once merged.
		std::cout << e.what() << '\n';		//	placeholder until a better solution is created, we should really use a log file instead
		return 0;

	}

}

void PPUClass::reset() {

	//	this currently follows the NTSC version of resetting found here: https://wiki.nesdev.com/w/index.php/PPU_power_up_state

	this->isOddFrame = false;

	// Reset all Registers.
	ResetRegister(this->registers.CTRL);
	ResetRegister(this->registers.MASK);
	ResetRegister(this->registers.STAT);
	
	// Clear out the OAM and nametable data
	std::fill(this->OAM.begin(), this->OAM.end(), 0x00);
	std::fill(this->nametables.begin(), this->nametables.end(), 0xFF);

	//	Also need to reset the scanline and current pixel
	this->scanline = 0;
	this->scanlinePixel = 0;

	// Also need to reset the pixels here, to ensure the screen is the proper size based on the type of ROM(NTSC vs. PAL)
	this->pixels.create(this->tvResolutionX[this->cartridge->getTV()], this->tvResolutionY[this->cartridge->getTV()]);

}

void PPUClass::cycle() {

	uint32_t resWidth, resHeight;
	std::tie(resWidth, resHeight) = this->getResolution();

	//	check if we are in the PRE phase
	if (scanline == -1)
		this->pre_scanline();
	//	grab the TV's height, this will tell us how many scanlines are drawn
	else if ((uint32_t)scanline < resHeight)
		frame_scanline();
	//	for the POST scanline, here we can update the GUI's frame to render
	else if ((uint32_t)scanline == (resHeight))
		post_scanline();
	//	for the VBLANK start of scanlines, the only thing that happens is on 241(second scanline after VISIBLE scanlines complete), setting the VBLANK flag
	else if ((uint32_t)scanline == (resHeight + 1))
		blank_scanline();

	//	check to see if the next scanline pixel is past the scanline size
	if (++(this->scanlinePixel) > 340) {

		//	keep the count within the proper size
		this->scanlinePixel %= 341;

		//	check if the next scanline is past the size, if so, reset
		if ((uint32_t)(++(this->scanline)) > (this->tvResolutionY[this->cartridge->getTV()] + 20))
			this->scanline = -1;

	}

}

void PPUClass::pre_scanline() {

	if (this->scanlinePixel == 1) {
		// Clear the Status register without disturbing the unused bits.
		ClearRegisterBits(this->registers.STAT, (STAT_BITMASKS::S_0_HIT | STAT_BITMASKS::S_OVERFLOW));
		ClearRegisterBits(this->registers.STAT, STAT_BITMASKS::VBLANK);
	}
	//	prerender piggybacks off the rendered, because the PPU still does accesses before the frame happens
	this->frame_scanline();

	// Copy vertical bits from t to v between pixels 280 and 304 in the pre-scanline
	// according to documentation at: https://wiki.nesdev.com/w/index.php/PPU_scrolling#During_dots_280_to_304_of_the_pre-render_scanline_.28end_of_vblank.29
	if (this->scanlinePixel >= 280 || this->scanlinePixel <= 304)
		this->vertical_update();
	//	according to documentation, we skip over the last pixel on the prerendered scanline only on odd frames: https://wiki.nesdev.com/w/index.php/PPU_rendering#Pre-render_scanline_.28-1_or_261.29
	else if (this->scanlinePixel == 340 && this->isOddFrame)
		this->scanlinePixel++;

}

void PPUClass::frame_scanline() {

	//	there are a total of 340 "pixels" to handle on a scanline, 240 are visible, the rest still perform tasks
	static uint16_t address = 0;

	//	this entire section basically takes care of what pixel is handled on each scanline within the visible frame: https://wiki.nesdev.com/w/images/d/d1/Ntsc_timing.png
	if ((this->scanlinePixel >= 2 && this->scanlinePixel <= 255) || (this->scanlinePixel >= 322 && this->scanlinePixel <= 337)) {

		this->pixelDraw();

		//	for the visible segment, there are only 8 different possibilities that can happen
		switch (this->scanlinePixel % 8) {

		//	NT reads cost 2 cycles, so first grab the address, then read from it
		case 1: address = this->addressNT(); this->reload_shift(); break;
		case 2: this->NT = this->access(address); break;

		//	same as NT, AT reads cost 2 cycles, uses Loopy's
		case 3: address = this->addressAT(); break;
		case 4:
			this->AT = this->access(address);
			if (this->vAddr.cY & 2)
				this->AT >>= 4;
			if (this->vAddr.cX & 2)
				this->AT >>= 2;
			break;

		// Background is broken up into 2 parts, so this will take 4 cycles to perform
		
		// Background lower byte
		case 5: address = this->addressBG(); break;
		case 6: this->BGL = this->access(address); break;

		// Background upper byte
		case 7: address += 8; break;
		case 0: this->BGH = this->access(address); this->horizontal_scroll(); break;

		}

	}
	else if (this->scanlinePixel == 1)
	{
		this->clear_oam();
		address = this->addressNT();
	}
	else if (this->scanlinePixel == 256) {
		// At pixel 256 we vertical scroll to wrap to the next nametable appropriately
		// For more info: https://wiki.nesdev.com/w/index.php/PPU_scrolling#At_dot_256_of_each_scanline
		this->pixelDraw();
		this->BGH = this->access(address);
		this->vertical_scroll();

	}
	else if (this->scanlinePixel == 257) {
		// At pixel 257 we copy the bits related to horizontal position from t to v.
		// For more info: https://wiki.nesdev.com/w/index.php/PPU_scrolling#At_dot_257_of_each_scanline
		this->evaluate_sprites();
		this->pixelDraw();
		this->reload_shift();
		this->horizontal_update();

	}
	else if (this->scanlinePixel == 321) {

		this->load_sprites();
		address = this->addressNT();

	}
	else if (this->scanlinePixel == 339)
		address = this->addressNT();
	else if (this->scanlinePixel == 338 || this->scanlinePixel == 340)
		this->NT = this->access(address);

}

void PPUClass::post_scanline() {

	//	we only need to update the frame once
	if (this->scanlinePixel == 0)
		this->GUI->updateFrame(this->pixels);

}

void PPUClass::blank_scanline() {

	if (this->scanlinePixel == 1) {

		// Set VBlank flag
		SetRegisterBits(this->registers.STAT, STAT_BITMASKS::VBLANK);
		
		if (this->registers.CTRL & CTRL_BITMASKS::NMI) // if NMI is set
		{
			this->CPU->setNMI(true);
		}
	}

}

void PPUClass::horizontal_scroll() {

	if (!this->rendering())
		return;

	if (this->vAddr.cX == 31)
		this->vAddr.r ^= 0x041F;
	else
		this->vAddr.cX++;

}

void PPUClass::vertical_scroll() {

	if (!this->rendering())
		return;

	if (this->vAddr.fY < 7)
		this->vAddr.fY++;
	else {

		this->vAddr.fY = 0;

		if (this->vAddr.cY == 31)
			this->vAddr.cY = 0;
		else if (this->vAddr.cY == 29) {

			this->vAddr.cY = 0;
			this->vAddr.nt ^= 2;

		}
		else
			this->vAddr.cY++;

	}

}

void PPUClass::reload_shift() {

	this->bgShiftL = (this->bgShiftL & 0xFF00) | (uint16_t)this->BGL;
	this->bgShiftH = (this->bgShiftH & 0xFF00) | (uint16_t)this->BGH;

	this->ATLatchL = (this->AT & 0b01);
	this->ATLatchH = (this->AT & 0b10);

}

void PPUClass::clear_oam() {

	for (uint8_t i = 0; i < 8; i++) {

		this->secondaryOAM[i].id = 0x40;
		this->secondaryOAM[i].data.y = 0xFF;
		this->secondaryOAM[i].data.index = 0xFF;
		this->secondaryOAM[i].data.attributes = 0xFF;
		this->secondaryOAM[i].data.x = 0xFF;
		this->secondaryOAM[i].dataL = 0x00;
		this->secondaryOAM[i].dataH = 0x00;

	}

}

void PPUClass::evaluate_sprites() {

	uint32_t n = 0;
	for (uint8_t i = 0; i < 64; i++) {

		int32_t line = scanline - this->OAM[i * 4];
		if (line >= 0 && line < this->sprite_height()) {

			this->secondaryOAM[n].id = i;
			this->secondaryOAM[n].data.y = this->OAM[i * 4];
			this->secondaryOAM[n].data.index = this->OAM[i * 4 + 1];
			this->secondaryOAM[n].data.attributes = this->OAM[i * 4 + 2];
			this->secondaryOAM[n].data.x = this->OAM[i * 4 + 3];

			if (++n >= 8) {
				// 8 or more sprites, so we set the Sprite Overflow bit in the Status register
				SetRegisterBits(this->registers.STAT, STAT_BITMASKS::S_OVERFLOW);
				break;

			}

		}

	}

}

void PPUClass::load_sprites() {

	uint16_t address = 0;
	for (uint8_t i = 0; i < 8; i++) {

		// Copy the secondary OAM to the primary OAM
		this->primaryOAM[i] = this->secondaryOAM[i];

		// We use a different addressing mode for sprites that are 16 pixels tall.
		if (this->sprite_height() == 16)
			address = ((this->primaryOAM[i].data.index & 1) * 0x1000) + ((this->primaryOAM[i].data.index & ~1) * 16);
		else
		{
			// Only the value of the Sprite bit in the control register matters.
			// Bitshift right by 3 because it is the 3rd bit in the control register.
			// Without the bitshift, it will multiply by 0x8000 instead of 0x1000
			address = (((this->registers.CTRL & CTRL_BITMASKS::SPRITE) >> 3) * 0x1000) + (this->primaryOAM[i].data.index * 16);
		}
		uint32_t spriteY = (this->scanline - this->primaryOAM[i].data.y) % this->sprite_height();
		if (this->primaryOAM[i].data.attributes & 0x80)
			spriteY ^= (this->sprite_height() - 1);
		address += spriteY + (spriteY & 8);

		this->primaryOAM[i].dataL = this->access(address);
		this->primaryOAM[i].dataH = this->access(address + 8); // Add 8 because we're storing into the higher order byte

	}

}

void PPUClass::pixelDraw() {

	uint8_t palette = 0, objPalette = 0;
	bool objPriority = false;
	int32_t x = this->scanlinePixel - 2;

	if ((this->scanline < this->tvResolutionY[this->cartridge->getTV()]) && (x >= 0) && (x < 256)) {

		//	background
		if (this->registers.MASK & MASK_BITMASKS::BG_ENABLE && !(!(this->registers.MASK & MASK_BITMASKS::BG_LEFT_COL) && (x < 8))) {

			palette = (((this->bgShiftH >> (15 - this->fineX)) & 1) << 1) | ((this->bgShiftL >> (15 - this->fineX)) & 1);
			if (palette)
				palette |= ((((this->atShiftH >> (7 - this->fineX)) & 1) << 1) | ((this->atShiftL >> (7 - this->fineX)) & 1)) << 2;

		}

		//	sprites
		if (this->registers.MASK & MASK_BITMASKS::S_ENABLE && !(!(this->registers.MASK & MASK_BITMASKS::S_LEFT_COL) && (x < 8))) {

			for (int8_t i = 7; i >= 0; i--) {

				if (this->primaryOAM[i].id == 0x40) continue;

				uint32_t spriteX = x - this->primaryOAM[i].data.x;
				if (spriteX >= 8) continue;
				if (this->primaryOAM[i].data.attributes & 0x40) spriteX ^= 7;

				uint8_t spritePalette = (((this->primaryOAM[i].dataH >> (7 - spriteX)) & 1) << 1) | ((this->primaryOAM[i].dataL >> (7 - spriteX)) & 1);
				if (spritePalette == 0) continue;

				if (this->primaryOAM[i].id == 0 && palette && x != 0xFF)
					this->registers.STAT |= STAT_BITMASKS::S_0_HIT;
				spritePalette |= (this->primaryOAM[i].data.attributes & 0x03) << 2;
				objPalette = spritePalette + 16;
				objPriority = this->primaryOAM[i].data.attributes & 0x20;

			}

		}

		if (objPalette && ((palette == 0) || (objPriority == 0))) palette = objPalette;

		uint16_t address = 0x3F00 + (this->rendering() ? palette : 0);
		uint8_t data = this->access(address);
		this->pixels.setPixel(x, this->scanline, sf::Color(this->paletteColors[data]));

	}

	this->bgShiftL <<= 1;
	this->bgShiftH <<= 1;
	this->atShiftL = (this->atShiftL << 1) | this->ATLatchL;
	this->atShiftH = (this->atShiftH << 1) | this->ATLatchH;

}

uint16_t PPUClass::addressMirrorAdjust(uint16_t addr) {

	switch (this->cartridge->getMirror()) {

	case mirrorEnum::HORIZONTAL: return ((addr / 2) & 0x0400) + (addr % 0x0400);
	case mirrorEnum::VERTICAL: return addr % 0x0800;
	default: return addr - 0x2000;

	}

}

void PPUClass::SetRegisterBits(uint8_t &ppuRegister, uint8_t ppuBitmask)
{
	ppuRegister |= ppuBitmask;
}

void PPUClass::ClearRegisterBits(uint8_t &ppuRegister, uint8_t ppuBitmask)
{
	ppuRegister &= ~ppuBitmask;
}

void PPUClass::ResetRegister(uint8_t &ppuRegister)
{
	ppuRegister = 0b00000000;
}