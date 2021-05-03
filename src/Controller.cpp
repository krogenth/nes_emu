#include ".\include\Controller.h"
#include ".\include\GUI.h"
#include <fstream>
#include <iostream>
ControllerClass::ControllerClass() {
	if (std::filesystem::exists("controller.ini")) {

		std::ifstream input("controller.ini");

		for (uint8_t i = 0; (i < 8) && !input.eof(); i++)
			input >> this->buttons[i];

		input.close();

	}
	else {

		this->buttons[0] = 0;
		this->buttons[1] = 1;
		this->buttons[2] = 2;
		this->buttons[3] = 3;
		this->buttons[4] = 4;
		this->buttons[5] = 5;
		this->buttons[6] = 6;
		this->buttons[7] = 7;
		this->setIni();

	}
}
ControllerClass::~ControllerClass() {

}
void ControllerClass::write(bool setStr) {
	//write data if strobe is set and being turned off
	if (strobe & !setStr) {
		PISO = this->GUI->getControllerState();
	}

	strobe = setStr;

}

uint8_t ControllerClass::read() {
	//while strobe is high (true) keep reading A
	if (strobe) {
		return 0x40 | (this->GUI->getControllerState() & 1);
	}
	//Gets the status of a button and shift the register
	uint8_t temp = 0x40 | (PISO & 1);
	PISO = 0x80 | (PISO >> 1);
	return temp;

}
uint8_t ControllerClass::getButton(int buttonIndex) {
	return ControllerClass::buttons[buttonIndex];
}

void ControllerClass::setButton(int buttonIndex, uint8_t controllerButton) {
	buttons[buttonIndex] = controllerButton;
}

void ControllerClass::setIni() {
	std::fstream iniFile("controller.ini", std::ios::out | std::ios::trunc);
	if (iniFile.is_open()) {
		for (uint8_t i = 0; i < 8; i++) {
			iniFile << this->buttons[i] << std::endl;
		}
		iniFile.close();
	}
}
