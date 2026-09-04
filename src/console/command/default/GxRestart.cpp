#include "console/command/Commands.hpp"
#include "util/Unimplemented.hpp"
#include <console/cvar/Gx.hpp>
#include <console/Console.hpp>
#include <console/Device.cpp>

// OFFSET: 0x769FF0
int32_t CCGxRestart(const char* command, const char* argument) {
    // sub_512900();

    ValidateFormatMonitor(s_requestedFormat);

    if (g_theGxDevicePtr->DeviceSetFormat(s_requestedFormat)) {
        memcpy(&s_lastGoodFormat, &s_requestedFormat, sizeof(CGxFormat));

        UpdateGxCVars();
    } else {
        ConsoleWrite("unable to set requested display mode", DEFAULT_COLOR);

        memcpy(&s_requestedFormat, &s_lastGoodFormat, sizeof(CGxFormat));

        if (g_theGxDevicePtr->DeviceSetFormat(s_requestedFormat)) {
            SetGxCVars(s_requestedFormat);
        } else {
            ConsoleWrite("unable to set last good mode", DEFAULT_COLOR);

            memcpy(&s_requestedFormat, &s_desktopFormat, sizeof(CGxFormat));

            int32_t set = g_theGxDevicePtr->DeviceSetFormat(s_requestedFormat);

            if (!set) {
                ConsoleWrite("unable to set default format", DEFAULT_COLOR);

                memcpy(&s_requestedFormat, &s_fallbackFormat, sizeof(CGxFormat));

                set = g_theGxDevicePtr->DeviceSetFormat(s_requestedFormat);
            }

            if (set) {
                memcpy(&s_lastGoodFormat, &s_requestedFormat, sizeof(CGxFormat));

                SetGxCVars(s_requestedFormat);
            }
        }
    }

    OsGuiSetGxWindow(GxDevWindow());

    OsGuiSetWindowTitle(GxDevWindow(), s_windowTitle);

    // TextureNotifyGxRestart();

    return 1;
}
