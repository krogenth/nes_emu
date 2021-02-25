#include "..\include\mappers\Mapper000.h"

//	for details on the mapper000 layout: https://wiki.nesdev.com/w/index.php/NROM
//	for rom listing of mapper000 roms: http://bootgod.dyndns.org:7777/search.php?ines=00

[[nodiscard]] uint8_t Mapper000::prg_read(uint16_t& address) {

	if (address >= 0x8000)
		/*
		for mapper000, there is only two possiblities, either 16KB(0x4000) of prg_rom or 32KB(0x8000) of prg_rom
		therefore, we can check how many chunks of prg_rom space we have, and adjust the address as needed
		*/
		return rom->prg_rom.at(((size_t)address - 0x8000) & ((rom->prg_rom_chunks > 1) ? (size_t)0x7FFF : (size_t)0x3FFF));
	else
		/*
		similar can be done with the prg_ram, which is either 2KB or 4KB only, and mirrored between 0x6000 and 0x7FFF
		this is only available for Family Basic, more can be read here: http://wiki.nesdev.com/w/index.php/NROM
		*/
		return rom->prg_ram.at(((size_t)address - (size_t)0x6000) & ((rom->prg_ram_chunks > 1) ? (size_t)0x0FFF : (size_t)0x07FF));

}

[[nodiscard]] uint8_t Mapper000::chr_read(uint16_t& address) {

	// only 8KB(0x2000) space of CHR ROM data, we can modulus this to ensure it stays within the proper space
	return rom->chr_rom.at((size_t)address & (size_t)0x1FFF);

}

uint8_t Mapper000::prg_write(uint16_t& address, const uint8_t& data) {

	if (address < 0x8000)
		//	there is meant to be a switch to disable all access to this space, but we'll assume it's non-existent
		return rom->prg_ram.at(((size_t)address - (size_t)0x6000) & ((rom->prg_ram_chunks > 1) ? (size_t)0x0FFF : (size_t)0x07FF)) = data;
	//	this else path is not required, because the prg_rom is read only
	//else
		//return rom->prg_rom[address & ((rom->prg_chunks > 1) ? 0x7FFF : 0x3FFF)] = data;

	return 0;

}

uint8_t Mapper000::chr_write(uint16_t& address, const uint8_t& data) {

	//	we can fill this is later once we know more about the PPU
	//	for Mapper000, there is no chr_ram, and chr_rom is read only, therefore it should only ever write to the PPU RAM
	return 0;

}