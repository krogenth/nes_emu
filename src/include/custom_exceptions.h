#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>	//	std::runtime_error
#include ".\include\ErrorLog.h"

class CartridgeException : public std::runtime_error {

public:
	CartridgeException(const std::string& message) throw() : std::runtime_error(message) {

		log.Write(message);

	}

private:
	ErrorLogClass log = ErrorLogClass();
};

class MapperException : public std::runtime_error {

public:
	MapperException(const std::string& message) throw() : std::runtime_error(message) {

		log.Write(message);

	}
private:
	ErrorLogClass log = ErrorLogClass();
};

#endif