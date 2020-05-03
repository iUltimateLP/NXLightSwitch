/*
    NXLightSwitch for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#include <switch.h>
#include "logger.hpp"

// Logs if the given result is not successful
#define LOG_IF_ERROR(r) if (R_FAILED(r)) { Logger::get()->logError(r); }
