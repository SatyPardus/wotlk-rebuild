#include "os/System.hpp"
#include <windows.h>

uint32_t OsGetNumberOfProcessors() {
    SYSTEM_INFO info = {};
    GetSystemInfo(&info);

    if (!info.dwNumberOfProcessors) {
        return 1;
    }

    return info.dwNumberOfProcessors;
}
