#ifndef TIME_C_GAME_TIME_HPP
#define TIME_C_GAME_TIME_HPP

#include "storm/hash/Hashkey.hpp"
#include "storm/hash/TSHashObject.hpp"
#include "storm/hash/TSHashTable.hpp"
#include "storm/list/TSExplicitList.hpp"
#include "storm/list/TSLink.hpp"
#include "time/WowTime.hpp"
#include <cstdint>

class CGameTime;

typedef void (*GAMETIMECALLBACK)(const WowTime& time, void* param);

class GAMETIMECBSTRUCT {
    public:
    // Member variables
    TSLink<GAMETIMECBSTRUCT> link;
    void* param;
    GAMETIMECALLBACK callback;
};

class TIMESTAMPSTRUCT : public TSHashObject<TIMESTAMPSTRUCT, HASHKEY_NONE> {
    public:
    // Member variables
    STORM_EXPLICIT_LIST(GAMETIMECBSTRUCT, link) callbacks;
};

class CGameTime : public WowTime {
    public:
    // Member variables
    int32_t unk_20;                                       // +0x20
    int32_t minuteOffset;                                 // +0x24
    int32_t dayOffset;                                    // +0x28
    int32_t minuteTicks;                                  // +0x2C
    float minutesPerSecond;                               // +0x30
    float fracMinutes;                                    // +0x34
    int32_t catchUpMinutes;                               // +0x38
    uint32_t lastTickMs;                                  // +0x3C
    bool running;                                         // +0x40
    float baseMinuteOfDay;                                // +0x44
    float unk_48;                                         // +0x48
    float unk_4C;                                         // +0x4C
    TSHashTable<TIMESTAMPSTRUCT, HASHKEY_NONE> callbacks; // +0x50

    // Member functions
    CGameTime();

    float GameTimeGetDayProgression();
    float GameTimeSetMinutesPerSecond(float minutesPerSecond);
    void GameTimeSetTime(const WowTime& time, bool fireCallbacks);
    void GameTimeSync(const WowTime& time, bool fireCallbacks);
    void GameTimeUpdate(float elapsedSec);
    GAMETIMECBSTRUCT* GameTimeRegisterCallback(const WowTime& time, GAMETIMECALLBACK callback, void* param);
    void GameTimeUnregisterCallback(GAMETIMECBSTRUCT* record);
    void TickMinute();
    void DispatchCallbacks(int32_t minuteOfDay);
};

extern CGameTime g_clientGameTime;

#endif
