#ifndef MAPPER_000_H
#define MAPPER_000_H

#include "Mapper.h"

class Mapper000 : public MapperClass {

public:
	Mapper000(romStruct* _rom) : MapperClass(_rom) {   }

	uint8_t prg_read(uint16_t& address) override;
	uint8_t chr_read(uint16_t& address) override;

	uint8_t prg_write(uint16_t& address, const uint8_t& data) override;
	uint8_t chr_write(uint16_t& address, const uint8_t& data) override;

protected:

};

#endif
