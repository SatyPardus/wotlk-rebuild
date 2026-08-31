#ifndef CLIENT_CLIENT_HPP
#define CLIENT_CLIENT_HPP

#include "event/Event.hpp"
#include "tempest/Vector.hpp"
#include "tempest/Random.hpp"
#include <cstdint>

class CVar;

namespace Client {
    extern CVar* g_accountNameVar;
    extern CVar* g_accountListVar;
    extern CVar* g_accountUsesTokenVar;
    extern CVar* g_movieVar;
    extern CVar* g_expansionMovieVar;
    extern CVar* g_movieSubtitleVar;
    extern CVar* g_lastCharacterIndex;
    extern CVar* g_desktopGamma;
    extern CVar* g_gamma;
    extern CVar* g_cvTextureFilteringMode;
    extern CVar* g_cvUIFaster;
    extern CVar* g_cvTextureCacheSize;
    extern HEVENTCONTEXT g_clientEventContext;
    extern char g_currentLocaleName[5];
}

extern bool g_hasIsoLocale[12];
extern const char* s_localeArray[12];

void ClientPostClose(int32_t a1);

const char* UpdateInstallLocation();

void CommonMain();

void StormInitialize();

void WowClientInit();

void ClientInitializeGame(int32_t zoneId, C3Vector* position);

#endif
