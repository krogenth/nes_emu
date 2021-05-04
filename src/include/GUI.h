#ifndef GUI_H
#define GUI_H

#include <vector>
#include <cinttypes>
#include <string>
#include <iostream>

#include ".\imgui.h"
#include ".\SFML\Graphics.hpp"
#include ".\SFML\System.hpp"
#include ".\SFML\Window\Joystick.hpp"

#include ".\CPU.h"
#include ".\PPU.h"
#include ".\Cartridge.h"
#include ".\ErrorLog.h"

#include ".\imfilebrowser.h"

struct MemoryEditor;

static bool showSelectDebug = false;
static bool showFileDialog = false;
static bool showControllerDialog = false;
static bool showButtonSet[]{ false, false, false, false};
static bool showMapperError = false;

//	structs to contain everything needed to create the windows for the hex viewers
//	split up based on component, because of the requirement for function pointers
struct CartridgeMemoryEditorContainer {

	MemoryEditor* hex_view = nullptr;
	std::string hex_name = "";
	CartridgeClass::GetCartData getDataFunc = nullptr;
	CartridgeClass::getCartDataSize getSizeFunc = nullptr;
	size_t hex_data_size = 0;
	bool isShown = false;

};

struct CPUMemoryEditorContainer {

	MemoryEditor* hex_view = nullptr;
	std::string hex_name = "";
	CPUClass::getCPUData getDataFunc = nullptr;
	CPUClass::getCPUDataSize getSizeFunc = nullptr;
	size_t hex_data_size = 0;
	bool isShown = false;

};

struct PPUMemoryEditorContainer {

	MemoryEditor* hex_view = nullptr;
	std::string hex_name = "";
	PPUClass::getPPUData getDataFunc = nullptr;
	PPUClass::getPPUDataSize getSizeFunc = nullptr;
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
	void addPPUViewer(std::string windowName, PPUClass::getPPUData getData_ptr, PPUClass::getPPUDataSize getSize_ptr);

	//	used to draw the current frame
	void draw();

	//	checks what keys are pressed through SFML, and stores in a byte to be compatible with the NES
	uint8_t getControllerState();

	//	helper functions to help determine if rendering should continue(the entire program) and if emulation is paused(CPU is paused)
	bool shouldRender() { return this->isRendering; }
	bool paused() { return this->isPaused; }

	//	loader functions of components the GUI interacts
	void loadCPU(CPUClass* _CPU) { this->CPU = _CPU; }
	void loadPPU(PPUClass* _PPU) { this->PPU = _PPU; }
	void loadCartridge(CartridgeClass* _cartridge) { this->cartridge = _cartridge; }
	void loadController(ControllerClass* _controller) { this->controller = _controller; }

	//	checks to see if there is a cartridge ROM loaded
	bool isLoaded();

	//	used to allow the PPU to update the current frame once scanline is complete
	void updateFrame(sf::Image& pixels);

private:
	bool isRendering = true;
	bool isPaused = false;
	bool showDebug = false;
	bool backgroundInput = false;

	uint32_t delay = 0;

	std::string progName = "";
	std::string loadedFile = "";

	ImGui::FileBrowser filebrowser;

	std::vector<CartridgeMemoryEditorContainer> cart_hex_windows;
	std::vector<CPUMemoryEditorContainer> cpu_hex_windows;
	std::vector<PPUMemoryEditorContainer> ppu_hex_windows;

	//	parts that we need for SFML
	sf::RenderWindow window;
	sf::Texture gameTexture;
	sf::Clock clock;

	//	pointers to components the GUI accesses
	CPUClass* CPU = nullptr;
	PPUClass* PPU = nullptr;
	CartridgeClass* cartridge = nullptr;
	ControllerClass* controller = nullptr;


	//	used to draw the menubar of the main window, called by draw
	void drawMenu();
	void drawSelectDebug();
	void drawDebug();
	void drawFileDialog();
	void drawControllerDialog();
	void drawSetButtons(int b);
	void drawErrorWindow();

	void saveGame();

};

#endif
