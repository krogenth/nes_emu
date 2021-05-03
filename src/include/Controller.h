#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cinttypes>

class GUIClass;

class ControllerClass {

public:
	ControllerClass();
	~ControllerClass();
	void write(bool setStr);
	uint8_t read();
	uint8_t getButton(int buttonIndex);
	void setButton(int buttonIndex, uint8_t controllerButton);
	void setIni();



	void loadGUI(GUIClass* _GUI) { this->GUI = _GUI; }



private:
	//	Parallel In, Serial Out storage of the controller
	//  set to 1 if the button was pressed 0 otherwise
	uint8_t PISO = 0b00000000;
	bool strobe = false;
	GUIClass* GUI = nullptr;
	uint8_t buttons[8];




};

#endif
