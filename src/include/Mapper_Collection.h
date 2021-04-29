#ifndef MAPPER_COLLECTION_H
#define MAPPER_COLLECTION_H

#include ".\mappers\Mapper.h"
#include ".\mappers\Mapper000.h"
#include ".\mappers\Mapper001.h"
#include ".\mappers\Mapper002.h"
#include ".\mappers\Mapper003.h"


#include ".\mappers\Mapper007.h"

#include ".\custom_exceptions.h"

MapperClass* selectMapper(romStruct* _rom);

#endif