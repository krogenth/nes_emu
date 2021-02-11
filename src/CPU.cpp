#include "include\CPU.h"

#include "include\Cartridge.h"

CPUClass::CPUClass(CartridgeClass* _cartridge) {

	this->RAM.resize(0x2000);
	this->cartridge = _cartridge;

}

CPUClass::~CPUClass() {



}

void CPUClass::cycle() {

	//	here we can check the system type(NTSC vs PAL) and call the PPU an equivalent number of times

}

void CPUClass::execute() {

	

}

uint8_t CPUClass::access(uint16_t address, uint8_t data, bool isWrite) {

	

}