// For timestamping error log messages
#include <ctime>
#include <fstream>

// Implementation for Error Logging in the NES EMU
#include ".\include\ErrorLog.h"


ErrorLogClass::ErrorLogClass()
{
	logFileName = DEFAULT_LOG_FILE_NAME;
}

ErrorLogClass::ErrorLogClass(std::string fileName)
{
	logFileName = fileName;
}

ErrorLogClass::~ErrorLogClass()
{

}

void ErrorLogClass::Write(std::string message)
{
	std::ofstream logFile;

	logFile.open(logFileName, std::ios_base::app);
	logFile << TimeStamp()<< message << std::endl;

	logFile.close();
}

std::string ErrorLogClass::GetLogFile()
{
	return logFileName;
}

void ErrorLogClass::SetLogFile(std::string fileName)
{
	logFileName = fileName;
}

bool ErrorLogClass::LogExists(std::string fileName)
{
	bool exists = false;
	std::ifstream file(fileName);

	if (file)
		exists = true;

	return exists;
}

std::string ErrorLogClass::TimeStamp()
{
	char str[26]; // Timestamps won't be larger than 26 characters
	
	time_t rawTime = time(NULL);

	struct tm curTime;
	localtime_s(&curTime, &rawTime);
	
	std::strftime(str, sizeof(str), "[%x - %I:%M%p] ", &curTime);

	std::string timeStr(str);
	return timeStr;
}

