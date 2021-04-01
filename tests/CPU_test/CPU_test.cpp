#include "..\..\src\include\CPU.h"

#include "..\..\src\include\Cartridge.h"
#include "..\..\src\include\Mapper_Collection.h"

#include "..\..\src\include\custom_exceptions.h"

int main() {

	CartridgeClass cartridge;
	CPUClass cpu;
	cartridge.storeMapper(selectMapper(cartridge.load("nestest.nes")));

	//	CPU needs to know about the cartridge, doesn't matter if a ROM is loaded or not
	cpu.loadCartridge(&cartridge);
	cpu.reset();	//	we need to power the CPU to a known state

	while (true)
		cpu.runFrame();

	return EXIT_SUCCESS;

}