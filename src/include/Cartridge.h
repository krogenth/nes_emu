#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <string>		//	std::string

#include ".\RomStruct.h"

//	forward declare romStruct
enum tvEnum : uint8_t;
struct romStruct;
class MapperClass;

class CartridgeClass {

public:
	~CartridgeClass() {  }
	[[nodiscard]] romStruct* load(std::string filename);
	void unload();
	bool loaded();
	void storeMapper(MapperClass* _mapper);
	void reset();
	uint8_t prg_access(uint16_t& address, const uint8_t& data, bool isWrite);	//	for CPU access
	uint8_t chr_access(uint16_t& address, const uint8_t& data, bool isWrite);	//	for PPU access

	mirrorEnum getMirror();
	tvEnum getTV();

	void* get_prm_rom();
	size_t get_prm_rom_size();

	void* get_prg_ram();
	size_t get_prg_ram_size();

	void* get_chr_rom();
	size_t get_chr_rom_size();

	//	function pointers for GUI to access Cartridge data
	typedef void* (CartridgeClass::*GetCartData)(void);
	typedef size_t(CartridgeClass::*getCartDataSize)(void);

private:
	MapperClass* mapper = nullptr;
	

};

#endif