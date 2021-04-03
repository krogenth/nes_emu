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

<<<<<<< Updated upstream
=======
	//	function pointers for GUI access to controller data
	typedef void* (ControllerClass::* getControllerData)(void);
	typedef size_t(ControllerClass::* getControllerDataSize)(void);

	void* get_controller_piso() { return (void*)&(this->PISO); }
	size_t get_controller_piso_size() { return 1; }

>>>>>>> Stashed changes


	void loadGUI(GUIClass* _GUI) { this->GUI = _GUI; }

<<<<<<< Updated upstream
	
=======

>>>>>>> Stashed changes

private:
	//	Parallel In, Serial Out storage of the controller
	//  set to 1 if the button was pressed 0 otherwise
	uint8_t PISO = 0b00000000;
	bool strobe = false;
	GUIClass* GUI = nullptr;
	uint8_t buttons[4];
<<<<<<< Updated upstream
	

	
=======



>>>>>>> Stashed changes

};

#endif