#include <fstream>
#include <iostream>

#include ".\SFML\Window\Keyboard.hpp"

#include ".\include\Controller.h"
#include ".\include\GUI.h"

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

		this->controllerButtons[0] = 0;		//	A
		this->controllerButtons[1] = 1;		//	B
		this->controllerButtons[2] = 2;		//	Select
		this->controllerButtons[3] = 3;		//	Start
		this->controllerButtons[4] = 4;		//	Up
		this->controllerButtons[5] = 5;		//	Down
		this->controllerButtons[6] = 6;		//	Left
		this->controllerButtons[7] = 7;		//	Right

		this->keyboardButtons[0] = sf::Keyboard::Key::R;	//	A
		this->keyboardButtons[1] = sf::Keyboard::Key::F;	//	B
		this->keyboardButtons[2] = sf::Keyboard::Key::T;	//	Select
		this->keyboardButtons[3] = sf::Keyboard::Key::G;	//	Start
		this->keyboardButtons[4] = sf::Keyboard::Key::W;	//	Up
		this->keyboardButtons[5] = sf::Keyboard::Key::S;	//	Down
		this->keyboardButtons[6] = sf::Keyboard::Key::A;	//	Left
		this->keyboardButtons[7] = sf::Keyboard::Key::D;	//	Right

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
