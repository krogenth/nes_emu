#include "include\CPU.h"

#include "include\Cartridge.h"

CPUClass::CPUClass() {

	this->RAM.resize(0x2000);

}

CPUClass::~CPUClass() {



}

void CPUClass::power() {

}

void CPUClass::reset() {



}

void CPUClass::cycle() {

	//	here we can check the system type(NTSC vs PAL) and call the PPU an equivalent number of times
	this->cycleCount++;

}

void CPUClass::execute() {

	//	grab the next opcode and increment the Program Counter to retrieve any data
	switch (this->access(this->reg_PC++)) {

	
	default: break;

	}

}

uint8_t CPUClass::access(uint16_t address, uint8_t data, bool isWrite) {

	if (address <= 0x1FFF) {

		return 0;

	}
	else if (address <= 0x3FFF) {

		//	PPU access
		return 0;

	}
	else if (address <= 0x4015) {

		//	APU access
		return 0;

	}
	else if (address == 0x4016) {

		//	APU access or joystick 1
		return 0;

	}
	else if (address == 0x4017) {

		//	APU access or joystick 2
		return 0;

	}
	else if (address <= 0x401F) {

		//	typically disabled APU/IO access
		return 0;

	}
	else
		return cartridge->prg_access(address, data, isWrite);

}