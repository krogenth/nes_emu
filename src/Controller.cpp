#include ".\include\Controller.h"

#include ".\include\GUI.h"

void ControllerClass::write(bool data) {

	//	Here we need to read the state of the keys pressed, and store them in the PISO shift register of the controller
	this->PISO = this->GUI->getControllerState();

}

uint8_t ControllerClass::read() {

	//	for reading, we only give the Most Significant Bit
	uint8_t temp = (this->PISO & 0x80) > 0;
	this->PISO <<= 1;
	return temp;

}