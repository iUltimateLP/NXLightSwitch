/*
    NXLightSwitch for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#pragma once
#include <cstdlib>
#include <ctime>

// This is the update interval for the worker thread (in nanoseconds)
#define WORKER_UPDATE_INTERVAL 1e+10

namespace nxlightswitch
{
    // This class implements the logic for the NXLightSwitch sysmodule.
    // It is ran by main.cpp in a dedicated thread.
    class Worker
    {
    public:
        // The main entry point for NXLightSwitch's logic. It will perform the rest.
        void DoWork();

    private:
        // Reads the configuration file of NXLightSwitch and stores the values
        bool ReadConfig();

        // Checks the console's current time and compares it with the stored light/dark time.
        // Also changes the theme accordingly
        void CheckForThemeChange();

    private:
        // Data read from the config
        struct std::tm lightTime;
        struct std::tm darkTime;
    };
}
