#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cinttypes>

class GUIClass;

class ControllerClass {

public:
	ControllerClass();
	~ControllerClass();

	//	functions for the CPU to read/write to/from the controller
	void write(bool setStr);
	uint8_t read();

	uint8_t getControllerButton(uint8_t buttonIndex);
	uint8_t getKeyboardButtons(uint8_t buttonIndex);
	void setControllerButton(uint8_t buttonIndex, uint8_t controllerButton);
	void setKeyboardButton(uint8_t buttonIndex, uint8_t keyboardButton);

	void setIni();

	void loadGUI(GUIClass* _GUI) { this->GUI = _GUI; }

private:
	//	Parallel In, Serial Out storage of the controller
	//  set to 1 if the button was pressed 0 otherwise
	uint8_t PISO = 0b00000000;
	bool strobe = false;
	GUIClass* GUI = nullptr;
	uint8_t controllerButtons[8];
	uint8_t keyboardButtons[8];

};

#endif
