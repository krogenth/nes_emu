#include "..\..\src\include\CPU.h"

#include "..\..\src\include\Cartridge.h"
#include "..\..\src\include\Mapper_Collection.h"

#include "..\..\src\include\custom_exceptions.h"

int main() {

	CartridgeClass cartridge;
	cartridge.storeMapper(selectMapper(cartridge.load("nestest.nes")));

	//	CPU needs to know about the cartridge, doesn't matter if a ROM is loaded or not
	CPUClass cpu;
	cpu.loadCartridge(&cartridge);
	cpu.power();	//	we need to power the CPU to a known state

	while (true)
		cpu.execute();

	return EXIT_SUCCESS;

}