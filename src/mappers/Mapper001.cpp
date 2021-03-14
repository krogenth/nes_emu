#include "..\include\mappers\Mapper001.h"

//	please see the following for the technical workings: https://wiki.nesdev.com/w/index.php/MMC1

Mapper001::Mapper001(romStruct* _rom) : MapperClass(_rom) {

	//	first window is either fixed to the first bank, or switchable
	this->prg_rom_window_1 = 0x00;
	//	second window is either fixed to the last bank, or switchable
	this->prg_rom_window_2 = this->rom->prg_rom_chunks - 1;

	//	the specification does not state where these windows are
	this->chr_rom_window_1 = 0x00;
	this->chr_rom_window_2 = 0x00;

}

[[nodiscard]] uint8_t Mapper001::prg_read(uint16_t& address) {

	//	following CPU address space, 0x6000 to 0x7FFF is PRG RAM space
	if (address < 0x8000)
		//	PRG RAM window size is only 8192(0x2000) in size
		return this->rom->prg_ram.at(((size_t)address - 0x6000) + ((size_t)this->prg_ram_window * 0x2000));
	else if (address < 0xC000)
		//	above 0x6000 and below 0xC000 is the first PRG ROM window section
		return this->rom->prg_rom.at(((size_t)address - 0x8000) + ((size_t)this->prg_rom_window_1 * 0x4000));
	else
		//	last, the second PRG ROM window section
		return this->rom->prg_rom.at(((size_t)address - 0xC000) + ((size_t)this->prg_rom_window_2 * 0x4000));

}

[[nodiscard]] uint8_t Mapper001::chr_read(uint16_t& address) {

	//	check if the address is in the first window section
	if (address < 0x1000)
		//	we want to window into a specific section at 4KB windows, so take the window offset, and multiply by 4096(0x1000)
		return this->rom->chr_rom.at((size_t)address + ((size_t)this->chr_rom_window_1 * 0x1000));
	else
		//	we need to reduce the address by 4096(0x1000) so that it is within the 4KB window space
		return this->rom->chr_rom.at((size_t)address - 0x1000 + ((size_t)this->chr_rom_window_2 * 0x1000));

}

uint8_t Mapper001::prg_write(uint16_t& address, const uint8_t& data) {

	if (address < 0x8000)
		//	PRG RAM section, we can write there
		return this->rom->prg_ram.at(((size_t)address - 0x6000) + ((size_t)this->prg_ram_window * 0x2000)) = data;

	if (data & 0x80) {	//	check if the highest bit of the daata was set, if so, reset the reg_Load

		this->clearLoad();
		this->reg_Control |= 0x0C;
		this->loadControl();
		return 0x00;	//	nothing else to do now, we can return

	}

	//	check if the LSB of the Load register is 1, this means the register is now full
	bool shouldCopy = (this->reg_Load & 0x01);
	//	we need to shift the register right 1 to store the next value
	this->reg_Load >>= 1;
	//	we need to store the next bit of data
	this->reg_Load |= (data & 0x01) << 0x04;

	if (shouldCopy) {

		if (address < 0xA000) {

			this->reg_Control = this->reg_Load;
			this->loadControl();

		}
		else if (address < 0xC000)
			this->chr_value_1 = this->reg_Load;
		else if (address < 0xE000)
			this->chr_value_2 = this->reg_Load;
		else
			this->prg_value_1 = this->reg_Load;

		this->updateWindows();
		this->clearLoad();

	}

	return 0;

}

uint8_t Mapper001::chr_write(uint16_t& address, const uint8_t& data) {

	//	check if the address is in the first window section
	if (address < 0x1000)
		//	we want to window into a specific section at 4KB windows, so take the window offset, and multiply by 4096(0x1000)
		return this->rom->chr_rom.at((size_t)address + ((size_t)this->chr_rom_window_1 * 0x1000)) = data;
	else
		//	we need to reduce the address by 4096(0x1000) so that it is within the 4KB window space
		return this->rom->chr_rom.at((size_t)address - 0x1000 + ((size_t)this->chr_rom_window_2 * 0x1000)) = data;

}

void Mapper001::loadControl() {

/*
4bit0
-----
CPPMM
|||||
|||++- Mirroring (0: one-screen, lower bank; 1: one-screen, upper bank;
|||               2: vertical; 3: horizontal)
|++--- PRG ROM bank mode (0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
|                         2: fix first bank at $8000 and switch 16 KB bank at $C000;
|                         3: fix last bank at $C000 and switch 16 KB bank at $8000)
+----- CHR ROM bank mode (0: switch 8 KB at a time; 1: switch two separate 4 KB banks)
*/

	this->prg_window_mode = (this->reg_Control & 0x0C) >> 2;
	this->chr_window_mode = (this->reg_Control & 0x10) >> 4;

	//	we need to update our windows after this
	this->updateWindows();

}

void Mapper001::updateWindows() {

	//	update PRG windows
	if (this->prg_window_mode <= 0x01) {

		//	we are using 1 32KB window
		//	set the first window to the even window, second to odd value
		this->prg_rom_window_1 = this->prg_value_1 & 0x0E;
		this->prg_rom_window_2 = this->prg_value_1 | 0x01;

	}
	else if (this->prg_window_mode == 0x02) {

		//	weare using 2 16KB windows, with the first fixed to the first bank
		this->prg_rom_window_1 = 0x00;	//	we use a fixed location
		this->prg_rom_window_2 = this->prg_value_1 & 0x0F;

	}
	else if (this->prg_window_mode == 0x03) {

		//	weare using 2 16KB windows, with the second fixed to the last bank
		this->prg_rom_window_1 = this->prg_value_1 & 0x0F;	//	we use a fixed location
		this->prg_rom_window_2 = this->rom->prg_rom_chunks - 1;

	}

	//	update CHR windows
	if (this->chr_window_mode == 0x00) {

		//	we are using 1 8KB windows, update both together
		this->chr_rom_window_1 = this->chr_value_1 & 0x0E;
		this->chr_rom_window_2 = this->chr_value_1 | 0x01;

	}
	else {

		//	we are using 2 4KB windows, update individually
		this->chr_rom_window_1 = this->chr_value_1;
		this->chr_rom_window_2 = this->chr_value_2;

	}

}