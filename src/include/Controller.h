#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cinttypes>

class GUIClass;

class ControllerClass {

public:
	void write();
	uint8_t read();

	void loadGUI(GUIClass* _GUI) { this->GUI = _GUI; }

private:
	//	Parallel In, Serial Out storage of the controller
	uint8_t PISO = 0b00000000;
	GUIClass* GUI = nullptr;

};

#endif
