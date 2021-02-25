#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <string>		//	std::string

//	forward declare romStruct
struct romStruct;
class MapperClass;

class CartridgeClass {

public:
	~CartridgeClass() {  }
	[[nodiscard]] romStruct* load(std::string filename);
	void unload();
	bool loaded() { return (mapper != nullptr); }
	void storeMapper(MapperClass* _mapper);
	void reset();
	uint8_t prg_access(uint16_t& address, const uint8_t& data, bool isWrite);	//	for CPU access
	uint8_t chr_access(uint16_t& address, const uint8_t& data, bool isWrite);	//	for PPU access

private:
	MapperClass* mapper = nullptr;

};

#endif