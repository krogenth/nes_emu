// Implementation for Error Logging in the NES EMU
#include "ErrorLog.h"


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

void ErrorLogClass::SetLogFile(std::string fileName)
{
	logFileName = fileName;
}

bool ErrorLogClass::LogExists(std::string fileName)
{
	// Not currently used. Maybe make a public function to check if a given filename already exists?
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

