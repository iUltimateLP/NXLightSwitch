/*
    NXLightSwitch for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#include "logger.hpp"
#include <switch.h>
using namespace nxlightswitch;

// Needed for compiler
Logger* Logger::singleton = NULL;

Logger* Logger::get()
{
    // If no singleton is existing, create a new instance
    if (!singleton)
    {
        singleton = new Logger();
    }

    // Return the instance
    return singleton;
}

void Logger::clearLogFile()
{
    // Open the file for write is all it needs to clear it
    FILE* logFile = fopen(LOG_FILE_PATH, "w");
    fflush(logFile);
    fclose(logFile);
}

void Logger::log(char* format, ...)
{
    // Format the log text buffer using variadic arguments
    char logBuffer[1024];
    va_list vaList;
    va_start(vaList, format);
    vsprintf(logBuffer, format, vaList);
    va_end(vaList);

    // Get the current time
    time_t currentTime;
    struct tm* timeInfo;
    time(&currentTime);
    timeInfo = localtime(&currentTime);

    // Format the time into a string
    char timeBuffer[80];
    strftime(timeBuffer, sizeof(timeBuffer), "%d-%m-%Y %H:%M:%S", timeInfo);

    // Open the log file
    FILE* logFile = fopen(LOG_FILE_PATH, "a"); // a means append

    // Print the log line to the file
    fprintf(logFile, "%s: %s\n", timeBuffer, logBuffer);
    
    // Flush and close the log file
    fflush(logFile);
    fclose(logFile);
}

void Logger::logError(uint32_t result)
{
    // Fancy formatting
    log("ERROR at %s:%s! Error code: %d", __FILE__, __LINE__, R_DESCRIPTION(result));
}
