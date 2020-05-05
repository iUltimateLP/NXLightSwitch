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

    // Get the current time (UNIX time, will always start at the UNIX Epoch 01/01/1970)
    time_t currentTime;
    struct std::tm* timeInfo;
    time(&currentTime);
    timeInfo = localtime(&currentTime);

    // Because of the not accurate result of the UNIX time(), try to get the console's time using libnx
    u64 currentConsoleTime;
    Result getTimeResult = timeGetCurrentTime(TimeType::TimeType_UserSystemClock, &currentConsoleTime);

    // If that worked...
    if (R_SUCCEEDED(getTimeResult))
    {
        // ...make a TimeCalendarTime (also from libnx) out of the timestamp
        TimeCalendarTime consoleCalendarTime;
        TimeCalendarAdditionalInfo consoleCalendarInfo;
        timeToCalendarTimeWithMyRule(currentConsoleTime, &consoleCalendarTime, &consoleCalendarInfo);

        // Update the values of our former timeInfo with the values we got from the Switch
        timeInfo->tm_mday = (int)consoleCalendarTime.day;
        timeInfo->tm_mon = (int)consoleCalendarTime.month - 1; // tm_mon is 0-based
        timeInfo->tm_year = (int)consoleCalendarTime.year - 1900; // tm_year is 0-based
        timeInfo->tm_hour = (int)consoleCalendarTime.hour;
        timeInfo->tm_min = (int)consoleCalendarTime.minute;
        timeInfo->tm_sec = (int)consoleCalendarTime.second;
    }

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
