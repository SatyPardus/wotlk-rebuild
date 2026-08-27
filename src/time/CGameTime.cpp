#include "time/CGameTime.hpp"
#include <bc/memory/Storm.hpp>
#include <common/time/Time.hpp>

// OFFSET: 0xD37F98
CGameTime g_clientGameTime;

// OFFSET: 0x76D600
CGameTime::CGameTime() {
    this->minutesPerSecond = 0.016666668f;
    this->fracMinutes = 0.0f;
    this->unk_20 = 0;
    this->baseMinuteOfDay = 0.0f;
    this->minuteOffset = 0;
    this->unk_48 = 0.0f;
    this->dayOffset = 0;
    this->unk_4C = 0.0f;
    this->minuteTicks = 0;
    this->catchUpMinutes = 0;
    this->lastTickMs = 0;
    this->running = false;
}

// OFFSET: 0x76CFF0
float CGameTime::GameTimeGetDayProgression() {
    int32_t elapsedMs = 0;

    if (this->running)
        elapsedMs = OsGetAsyncTimeMs() - this->lastTickMs;

    float minutes = elapsedMs * 0.001f * this->minutesPerSecond + this->baseMinuteOfDay;

    while (minutes > 1440.0f)
        minutes -= 1440.0f;

    return minutes * 0.00069444446f;
}

// OFFSET: 0x76CFA0
float CGameTime::GameTimeSetMinutesPerSecond(float minutesPerSecond) {
    float previous = this->minutesPerSecond;

    if (minutesPerSecond > 60.0f)
        minutesPerSecond = 60.0f;
    else if (minutesPerSecond < 0.016666668f)
        minutesPerSecond = 0.016666668f;

    this->minutesPerSecond = minutesPerSecond;
    return previous;
}

// OFFSET: 0x76D740
void CGameTime::TickMinute() {
    int32_t minuteOfDay = (this->GetHourAndMinutes() + 1) % 1440;

    this->SetHourAndMinutes(minuteOfDay);

    if (!minuteOfDay)
        this->AddDays(1, false);

    this->minuteTicks++;

    this->DispatchCallbacks(minuteOfDay);

    this->lastTickMs = OsGetAsyncTimeMs();
    this->running = true;
    this->baseMinuteOfDay = minuteOfDay + this->fracMinutes;
}

// OFFSET: 0x76D650
void CGameTime::DispatchCallbacks(int32_t minuteOfDay) {
    TIMESTAMPSTRUCT* bucket = this->callbacks.Ptr(minuteOfDay, HASHKEY_NONE());
    if (!bucket)
        return;

    for (GAMETIMECBSTRUCT* record = bucket->callbacks.Head(); record;) {
        GAMETIMECBSTRUCT* next = bucket->callbacks.Next(record);
        record->callback(*this, record->param);
        record = next;
    }
}

// OFFSET: 0x76D900
void CGameTime::GameTimeUpdate(float elapsedSec) {
    this->fracMinutes = this->minutesPerSecond * elapsedSec + this->fracMinutes;

    if (this->catchUpMinutes && this->fracMinutes >= 1.0f) {
        uint32_t swallowed = static_cast<uint32_t>(this->fracMinutes);

        if (static_cast<uint32_t>(this->catchUpMinutes) < swallowed)
            swallowed = this->catchUpMinutes;

        this->catchUpMinutes -= swallowed;
        this->fracMinutes -= swallowed;
    }

    while (this->fracMinutes >= 1.0f) {
        this->fracMinutes -= 1.0f;
        this->TickMinute();
    }
}

// OFFSET: 0x76D810
void CGameTime::GameTimeSetTime(const WowTime& time, bool fireCallbacks) {
    WowTime adjusted = time;
    adjusted.tzHours = time.tzHours;

    if (this->minuteOffset) {
        int32_t minuteOfDay = this->minuteOffset + adjusted.GetHourAndMinutes();

        if (minuteOfDay >= 0) {
            minuteOfDay %= 1440;
        } else {
            minuteOfDay += 1440;
        }

        adjusted.SetHourAndMinutes(minuteOfDay);
    }

    if (this->dayOffset)
        adjusted.AddDays(this->dayOffset, false);

    adjusted.tzHours = time.tzHours;
    *static_cast<WowTime*>(this) = adjusted;

    if (!fireCallbacks)
        return;

    // Step back one minute so that TickMinute lands on the requested time and
    // runs the callbacks registered for it.
    if (this->minute) {
        this->minute--;
    } else {
        this->minute = 59;

        if (this->hour) {
            this->hour--;
        } else {
            this->hour = 23;
            this->AddDays(-1, false);
        }
    }

    this->TickMinute();
}

// OFFSET: 0x76D9A0
void CGameTime::GameTimeSync(const WowTime& time, bool fireCallbacks) {
    WowTime adjusted = time;
    adjusted.tzHours = time.tzHours;

    if (this->minuteOffset) {
        int32_t minuteOfDay = this->minuteOffset + adjusted.GetHourAndMinutes();

        if (minuteOfDay >= 0) {
            minuteOfDay %= 1440;
        } else {
            minuteOfDay += 1440;
        }

        adjusted.SetHourAndMinutes(minuteOfDay);
    }

    if (this->dayOffset)
        adjusted.AddDays(this->dayOffset, false);

    int32_t dayDelta = 1440 * (adjusted.GetDaysSinceEpoch() - this->GetDaysSinceEpoch());
    int32_t delta = (dayDelta - this->GetHourAndMinutes() + adjusted.GetHourAndMinutes()) % 1440;

    int32_t behind;

    if (fireCallbacks || delta > 0) {
        // if (delta > 1) {
        //     char adjustedText[1024];
        //     char currentText[1024];
        //     WowTime::WowGetTimeString(&adjusted, adjustedText, 1024);
        //     WowTime::WowGetTimeString(this, currentText, 1024);
        //     SysMsgPrintf(10, "GameTimeSync: skipping forwards %d game minutes, from %s to %s", delta, currentText, adjustedText);
        // }

        while (delta > 0) {
            this->TickMinute();
            delta--;
        }

        behind = 0;
    } else {
        behind = -delta;
    }

    adjusted.tzHours = time.tzHours;
    *static_cast<WowTime*>(this) = adjusted;

    if (!behind)
        return;

    // The client is ahead of the server. Rather than run the clock backwards
    // and replay callbacks, run it forwards and stall for the same number of
    // minutes until the server catches up.
    this->catchUpMinutes += behind;

    for (int32_t remaining = behind; remaining > 0; remaining--)
        this->TickMinute();

    // SysMsgPrintf(10, "GameTimeSync: delta=%d, differential=%d, current=%d", 0, this->catchUpMinutes, this->GetHourAndMinutes());
}

// OFFSET: 0x76DB30
GAMETIMECBSTRUCT* CGameTime::GameTimeRegisterCallback(const WowTime& time, GAMETIMECALLBACK callback, void* param) {
    if (!callback) {
        // SErrSetLastError(ERROR_INVALID_PARAMETER);
        return nullptr;
    }

    if (time.hour < 0 || time.minute < 0)
        return nullptr;

    int32_t minuteOfDay = time.GetHourAndMinutes();

    TIMESTAMPSTRUCT* bucket = this->callbacks.Ptr(minuteOfDay, HASHKEY_NONE());
    if (!bucket) {
        bucket = this->callbacks.New(minuteOfDay, HASHKEY_NONE(), 0, 0);
        bucket->m_hashval = minuteOfDay;
    }

    GAMETIMECBSTRUCT* record = STORM_NEW_ZERO(GAMETIMECBSTRUCT);
    if (!record)
        return nullptr;

    record->link.Unlink();
    bucket->callbacks.LinkToHead(record);

    record->param = param;
    record->callback = callback;
    return record;
}

// OFFSET: 0x76D1D0
void CGameTime::GameTimeUnregisterCallback(GAMETIMECBSTRUCT* record) {
    record->link.Unlink();
    STORM_FREE(record);
}
