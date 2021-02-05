#ifndef MAPPER_COLLECTION_H
#define MAPPER_COLLECTION_H

#include "mappers\Mapper.h"
#include "mappers\Mapper000.h"
#include "mappers\Mapper001.h"

#include "..\include\custom_exceptions.h"

MapperClass* selectMapper(romStruct* _rom);

#endif