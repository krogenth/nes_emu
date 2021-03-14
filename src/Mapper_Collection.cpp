#include ".\include\Mapper_Collection.h"

MapperClass* selectMapper(romStruct* _rom) {

	MapperClass* mapper = nullptr;

	uint8_t mapperID = (_rom->header.flags7 & 0b11110000) | ((_rom->header.flags6 & 0b11110000) >> 4);
	switch (mapperID) {

	case 0:
		mapper = new Mapper000(_rom);
		break;
	case 1:
		mapper = new Mapper001(_rom);
		break;
	case 2:
		mapper = new Mapper002(_rom);
		break;
	default:
		delete _rom;	//	rom is useless, just delete it
		throw MapperException("MapperID " + std::to_string(mapperID) + " not supported");

	}

	return mapper;

}