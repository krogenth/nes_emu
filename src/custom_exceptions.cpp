#include ".\include\ErrorLog.h"
#include ".\include\custom_exceptions.h"

CartridgeException::CartridgeException(const std::string& message) throw() : std::runtime_error(message) {

	ErrorLogClass log = ErrorLogClass();
	log.Write(message);
}

MapperException::MapperException(const std::string& message) throw() : std::runtime_error(message) {

	ErrorLogClass log = ErrorLogClass();
	log.Write(message);
}