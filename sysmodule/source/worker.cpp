/*
    NXLightSwitch for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#include "worker.hpp"
#include "logger.hpp"
#include "ini/inireader.hpp"
#include <switch.h>
using namespace nxlightswitch;

void Worker::DoWork()
{
    // 1. Read the config if it changed to see if we got any new times
    if (!ReadConfig())
        return;

    // 2. If the config enabled automatic day/night theming, and it's a new day, check 
    //    the public API to grab the latest sunrise/sunset times

    // 3. Compare the times from the config with the current time to see if we should
    //    update the systems theme

    // 4. Update the systems theme
}

bool Worker::ReadConfig()
{
    // Create a new INIReader to read the config file
    INIReader iniReader("sdmc:/NXLightSwitch.ini");

    // Make sure we were able to read the ini file
    if (iniReader.ParseError() < 0)
    {
        Logger::get()->log("Error loading config file! Error code: %d", iniReader.ParseError());
        return false;
    }

    // Read the values off the config
    std::string lightTimeStr = iniReader.GetString("NXLightSwitch", "LightTime", "0");
    std::string darkTimeStr = iniReader.GetString("NXLightSwitch", "DarkTime", "0");
    useAutomaticTimes = iniReader.GetBoolean("NXLightSwitch", "Automatic", false);

    // Convert the string timestamps into time_t timestamps
    lightTime = (std::time_t)strtol(lightTimeStr.c_str(), NULL, 10);
    darkTime = (std::time_t)strtol(darkTimeStr.c_str(), NULL, 10);

    return true;
}