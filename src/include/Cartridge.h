#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <string>		//	std::string

//	forward declare romStruct
struct romStruct;
class Mapper;

class CartridgeClass {

public:
	[[nodiscard]] Mapper* load(std::string filename);

private:
	Mapper* mapper = nullptr;

};

#endif