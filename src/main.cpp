#include <string>

#include "..\..\src\include\CPU.h"
#include "..\..\src\include\PPU.h"
#include "..\..\src\include\Cartridge.h"
#include "..\..\src\include\Controller.h"
#include "..\..\src\include\GUI.h"

#include "..\..\src\include\custom_exceptions.h"

const std::string PROG_NAME = "nes_emu";

int main(int argc, char* argv[]) {

	CPUClass cpu;
	PPUClass ppu;
	ControllerClass controller;
	CartridgeClass cartridge;
	GUIClass gui(PROG_NAME);

	//	give CPU access to relevant components it needs to access
	//	CPU needs to know about the cartridge, doesn't matter if a ROM is loaded or not
	cpu.loadPPU(&ppu);
	cpu.loadCartridge(&cartridge);
	cpu.loadController(&controller);

	ppu.loadCartridge(&cartridge);
	ppu.loadGUI(&gui);
	ppu.loadCPU(&cpu);

	//	give GUI access to relevant components it needs to access
	gui.loadCPU(&cpu);
	gui.loadPPU(&ppu);
	gui.loadCartridge(&cartridge);

	//	give the GUI the information it needs in order to create the windows for the various memory spaces
	gui.addCPUViewer("CPU RAM", &CPUClass::get_cpu_ram, &CPUClass::get_cpu_ram_size);
	gui.addCPUViewer("CPU Regs", &CPUClass::get_cpu_regs, &CPUClass::get_cpu_regs_size);

	gui.addCartViewer("Cartridge PRG ROM", &CartridgeClass::get_prm_rom, &CartridgeClass::get_prm_rom_size);
	gui.addCartViewer("Cartridge PRG RAM", &CartridgeClass::get_prg_ram, &CartridgeClass::get_prg_ram_size);
	gui.addCartViewer("Cartridge CHR ROM", &CartridgeClass::get_chr_rom, &CartridgeClass::get_chr_rom_size);

	gui.addPPUViewer("PPU Nametables",&PPUClass::get_ppu_nametables,&PPUClass::get_ppu_nametables_size);
	gui.addPPUViewer("PPU Regs", &PPUClass::get_ppu_regs, &PPUClass::get_ppu_regs_size);
	gui.addPPUViewer("PPU OAM", &PPUClass::get_ppu_oam, &PPUClass::get_ppu_oam_size);
	gui.addPPUViewer("PPU Primary OAM", &PPUClass::get_ppu_primary_oam, &PPUClass::get_ppu_oam_reg_size);
	gui.addPPUViewer("PPU Secondary OAM", &PPUClass::get_ppu_secondary_oam, &PPUClass::get_ppu_oam_reg_size);
	gui.addPPUViewer("PPU Palettes", &PPUClass::get_ppu_palettes, &PPUClass::get_ppu_palettes_size);

	while (gui.shouldRender()) {

		if (!gui.paused() && gui.isLoaded())
			cpu.runFrame();

		gui.draw();

	}

	return EXIT_SUCCESS;

}