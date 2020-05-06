/*
    NXLightSwitch for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#include "worker.hpp"
#include "logger.hpp"
#include "ini/inireader.hpp"
#include <iomanip>
#include <sstream>
#include <switch.h>
using namespace nxlightswitch;

void Worker::DoWork()
{
    // 1. Read the config if it changed to see if we got any new times
    if (!ReadConfig())
        return;

    // 2. Compare the times from the config with the current time to see if we should
    //    update the systems theme
    CheckForThemeChange();
}

bool Worker::ReadConfig()
{
    // Create a new INIReader to read the config file
    INIReader iniReader("sdmc:/config/NXLightSwitch/NXLightSwitch.ini");

    // Make sure we were able to read the ini file
    if (iniReader.ParseError() < 0)
    {
        Logger::get()->log("Error loading config file! Error code: %d", iniReader.ParseError());
        return false;
    }

    // Read the values off the config
    std::string lightTimeStr = iniReader.GetString("NXLightSwitch", "LightTime", "0");
    std::string darkTimeStr = iniReader.GetString("NXLightSwitch", "DarkTime", "0");

    // Convert the string times into time_t times
    // Note: the times need to be in the following format: HH:MM (see http://www.cplusplus.com/reference/ctime/strftime/)
    lightTime = {0};
    std::istringstream ssLight(lightTimeStr);
    ssLight >> std::get_time(&lightTime, "%R");

    darkTime = {0};
    std::istringstream ssDark(darkTimeStr);
    ssDark >> std::get_time(&darkTime, "%R");

    //Logger::get()->log("Light: %d:%d - Dark: %d:%d", lightTime.tm_hour, lightTime.tm_min, darkTime.tm_hour, darkTime.tm_min);

    return true;
}

void Worker::CheckForThemeChange()
{
    // Get the current time of the Nintendo Switch console using libnx
    u64 currentConsoleTime;
    Result getTimeResult = timeGetCurrentTime(TimeType::TimeType_UserSystemClock, &currentConsoleTime);

    // Make sure we were able to get the time
    if (R_FAILED(getTimeResult))
    {
        Logger::get()->logError(getTimeResult);
        return;
    }

    // Make a TimeCalendarTime out of the timestamp using libnx' features
    TimeCalendarTime consoleCalendarTime;
    TimeCalendarAdditionalInfo consoleCalendarInfo;
    timeToCalendarTimeWithMyRule(currentConsoleTime, &consoleCalendarTime, &consoleCalendarInfo);

    // Now perform some logic checks to see if we should change the theme
    bool needsLightThemeChange = consoleCalendarTime.hour >= lightTime.tm_hour 
        && consoleCalendarTime.minute >= lightTime.tm_min
        && ((consoleCalendarTime.hour > darkTime.tm_hour && (consoleCalendarTime.minute > darkTime.tm_min || consoleCalendarTime.minute == 0)) 
            || (consoleCalendarTime.hour == darkTime.tm_hour && consoleCalendarTime.minute > darkTime.tm_min));

    bool needsDarkThemeChange = consoleCalendarTime.hour >= darkTime.tm_hour
        && consoleCalendarTime.minute >= darkTime.tm_min
        && ((consoleCalendarTime.hour < lightTime.tm_hour && (consoleCalendarTime.minute < lightTime.tm_min || consoleCalendarTime.minute == 0)) 
            || (consoleCalendarTime.hour == lightTime.tm_hour && consoleCalendarTime.minute < lightTime.tm_min));

    bool needsThemeChange = needsLightThemeChange || needsDarkThemeChange;

    ColorSetId currentTheme;
    Result sysGetColorSetIdResult = setsysGetColorSetId(&currentTheme);
    if (R_SUCCEEDED(sysGetColorSetIdResult))
    {
        Logger::get()->log("Got color theme");
    }
    else
    {
        Logger::get()->logError(sysGetColorSetIdResult);
    }

    Logger::get()->log("CheckForThemeChange() CurrentTime = %d:%d CurrentTheme = %s NeedsLightThemeChange = %d (%d:%d) NeedsDarkThemeChange = %d (%d:%d)",
        consoleCalendarTime.hour,
        consoleCalendarTime.minute,
        currentTheme == ColorSetId::ColorSetId_Light ? "Light" : "Dark",
        needsLightThemeChange,
        lightTime.tm_hour,
        lightTime.tm_min,
        needsDarkThemeChange,
        darkTime.tm_hour,
        darkTime.tm_min);

    // Do we need to change the theme?
    if (needsThemeChange)
    {
        // What will the new theme be?
        ColorSetId newTheme = needsLightThemeChange ? ColorSetId::ColorSetId_Light : ColorSetId::ColorSetId_Dark;

        // Apply the new theme using libnx
        Result sysSetColorSetIdResult = setsysSetColorSetId(newTheme);

        // Check if it worked
        if (R_SUCCEEDED(sysSetColorSetIdResult))
        {
            Logger::get()->log("Changed theme to %s", newTheme == ColorSetId::ColorSetId_Light ? "Light" : "Dark");
        }
        else
        {
            Logger::get()->logError(sysSetColorSetIdResult);
        }
    }
}
