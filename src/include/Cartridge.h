#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <string>		//	std::string

//	forward declare romStruct
struct romStruct;
class MapperClass;

class CartridgeClass {

public:
	[[nodiscard]] romStruct* load(std::string filename);
	bool loaded() { return (mapper != nullptr); }
	void storeMapper(MapperClass* _mapper) { mapper = _mapper; }
	void reset();

private:
	MapperClass* mapper = nullptr;

};

#endif