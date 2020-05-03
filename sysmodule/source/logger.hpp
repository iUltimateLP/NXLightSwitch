/*
    NXLightSwitch for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#pragma once
#include <fstream>
#include <cstdarg>
#include <ctime>

namespace nxlightswitch
{
    // This class implements a logging system to easily log to a file,
    // which is very helpful during development
    class Logger
    {
    public:
        // Returns the singleton instance of this logger
        static Logger* get();

        // Logs with variadic arguments
        void log(char* format, ...);

        // Logs an libnx error
        void logError(uint32_t result);

    private:
        // Singleton instance
        static Logger* singleton;
    };
}
