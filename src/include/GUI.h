#ifndef GUI_H
#define GUI_H

#include <vector>
#include <cinttypes>
#include <string>
#include <iostream>

#include ".\imgui.h"
#include ".\SDL.h"

#include ".\CPU.h"
#include ".\PPU.h"
#include ".\Cartridge.h"

#include ".\imfilebrowser.h"

struct MemoryEditor;

static bool showSelectDebug = false;
static bool showFileDialog = false;

//	structs to contain everything needed to create the windows for the hex viewers
//	split up based on component, because of the requirement for function pointers
struct CartridgeMemoryEditorContainer {

	MemoryEditor* hex_view;
	std::string hex_name = "";
	CartridgeClass::GetCartData getDataFunc = nullptr;
	CartridgeClass::getCartDataSize getSizeFunc = nullptr;
	size_t hex_data_size = 0;
	bool isShown = false;

};

struct CPUMemoryEditorContainer {

	MemoryEditor* hex_view;
	std::string hex_name = "";
	CPUClass::getCPUData getDataFunc = nullptr;
	CPUClass::getCPUDataSize getSizeFunc = nullptr;
	size_t hex_data_size = 0;
	bool isShown = false;

};

class GUIClass {

public:
	GUIClass(std::string progName);
	~GUIClass();

	//	we need to have different functions for the different classes to view, because we need to use function pointers to view the data
	void addCartViewer(std::string windowName, CartridgeClass::GetCartData getData_ptr, CartridgeClass::getCartDataSize getSize_ptr);
	void addCPUViewer(std::string windowName, CPUClass::getCPUData getData_ptr, CPUClass::getCPUDataSize getSize_ptr);

	//	used to draw the current frame
	void draw();

	//	checks what keys are pressed through SDL2, and stores in a byte to be compatible with the NES
	uint8_t getControllerState();

	//	helper functions to help determine if rendering should continue(the entire program) and if emulation is paused(CPU is paused)
	bool shouldRender() { return this->isRendering; }
	bool paused() { return this->isPaused; }

	//	loader functions of components the GUI interacts
	void loadCPU(CPUClass* _CPU) { this->CPU = _CPU; }
	void loadPPU(PPUClass* _PPU) { this->PPU = _PPU; }
	void loadCartridge(CartridgeClass* _cartridge) { this->cartridge = _cartridge; }

	//	checks to see if there is a cartridge ROM loaded
	bool isLoaded();

	//	used to allow the PPU to update the current frame once scanline is complete
	void updateFrame(uint32_t* pixels);

private:
	bool isRendering = true;
	bool isPaused = false;
	bool showDebug = false;

	std::string loadedFile = "";

	ImGui::FileBrowser filebrowser;

	std::vector<CartridgeMemoryEditorContainer> cart_hex_windows;
	std::vector<CPUMemoryEditorContainer> cpu_hex_windows;

	//	SDL2 and ImGui specific types used for handling rendering
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_GLContext gl_context;
	SDL_Texture* gameTexture = nullptr;

	//	pointers to components the GUI accesses
	CPUClass* CPU = nullptr;
	PPUClass* PPU = nullptr;
	CartridgeClass* cartridge = nullptr;

	//	used to draw the menubar of the main window, called by draw
	void drawMenu();
	void drawSelectDebug();
	void drawDebug();
	void drawFileDialog();

};

#endif