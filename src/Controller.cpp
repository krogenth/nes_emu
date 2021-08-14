#include ".\include\Controller.h"
#include ".\include\GUI.h"
#include <fstream>
#include <iostream>
ControllerClass::ControllerClass() {

	if (std::filesystem::exists("controller.ini")) {

		std::ifstream input("controller.ini");

		for (uint8_t i = 0; (i < 8) && !input.eof(); i++)
			input >> this->controllerButtons[i];

		for (uint8_t i = 0; (i < 8) && !input.eof(); i++)
			input >> this->keyboardButtons[i];

		input.close();

	}
	else {

		this->controllerButtons[0] = 0;
		this->controllerButtons[1] = 1;
		this->controllerButtons[2] = 2;
		this->controllerButtons[3] = 3;
		this->controllerButtons[4] = 4;
		this->controllerButtons[5] = 5;
		this->controllerButtons[6] = 6;
		this->controllerButtons[7] = 7;
		this->setIni();

	}

}

ControllerClass::~ControllerClass() {



}

void ControllerClass::write(bool setStr) {

	//write data if strobe is set and being turned off
	if (strobe && !setStr) {

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

uint8_t ControllerClass::getControllerButton(uint8_t buttonIndex) {

	return ControllerClass::controllerButtons[buttonIndex];

}

uint8_t ControllerClass::getKeyboardButtons(uint8_t buttonIndex) {

	return ControllerClass::keyboardButtons[buttonIndex];

}

void ControllerClass::setControllerButton(uint8_t buttonIndex, uint8_t controllerButton) {

	this->controllerButtons[buttonIndex] = controllerButton;

}

void ControllerClass::setKeyboardButton(uint8_t buttonIndex, uint8_t controllerButton) {

	this->keyboardButtons[buttonIndex] = controllerButton;

}

void ControllerClass::setIni() {

	std::fstream iniFile("controller.ini", std::ios::out | std::ios::trunc);
	if (iniFile.is_open()) {

		for (uint8_t i = 0; i < 8; i++) {

			iniFile << this->controllerButtons[i] << std::endl;

		}

		for (uint8_t i = 0; i < 8; i++) {

			iniFile << this->keyboardButtons[i] << std::endl;

		}

		iniFile.close();

	}

}
