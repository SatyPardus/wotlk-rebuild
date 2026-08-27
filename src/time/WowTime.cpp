#include "time/WowTime.hpp"
#include <common/datastore/CDataStore.hpp>
#include <ctime>
#include <storm/String.hpp>

static const char* const s_weekDayNames[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

// OFFSET: 0x76C1B0
static void LocalTimeSafe(const time_t* time, struct tm* result) {
    struct tm* local = localtime(time);
    if (local) {
        *result = *local;
    } else {
        result->tm_sec = 0;
        result->tm_min = 0;
        result->tm_hour = 0;
        result->tm_mday = 0;
        result->tm_mon = 0;
        result->tm_year = 0;
        result->tm_wday = 0;
        result->tm_yday = 0;
        result->tm_isdst = 0;
    }
}

// OFFSET: 0x76C190
WowTime::WowTime() {
    this->minute = -1;
    this->hour = -1;
    this->weekDay = -1;
    this->monthDay = -1;
    this->month = -1;
    this->year = -1;
    this->flags = 0;
    this->tzHours = 0;
}

// OFFSET: 0x76C910
void WowTime::WowEncodeTime(uint32_t& packed, int32_t minute, int32_t hour, int32_t weekDay, int32_t monthDay, int32_t month, int32_t year, int32_t flags) {
    packed = (minute & 0x3F) | ((hour & 0x1F) << 6) | ((weekDay & 0x7) << 11) | ((monthDay & 0x3F) << 14) | ((month & 0xF) << 20) | ((year & 0x1F) << 24) | ((flags & 0x3) << 29);
}

// OFFSET: 0x76CA50
void WowTime::WowEncodeTime(uint32_t& packed, const WowTime* time) {
    WowTime::WowEncodeTime(packed, time->minute, time->hour, time->weekDay, time->monthDay, time->month, time->year, time->flags);
}

// OFFSET: 0x76C970
void WowTime::WowDecodeTime(uint32_t packed, int32_t* minute, int32_t* hour, int32_t* weekDay, int32_t* monthDay, int32_t* month, int32_t* year, int32_t* flags) {
    if (minute) {
        uint32_t value = packed & 0x3F;
        *minute = value == 0x3F ? -1 : value;
    }

    if (hour) {
        uint32_t value = (packed >> 6) & 0x1F;
        *hour = value == 0x1F ? -1 : value;
    }

    if (weekDay) {
        uint32_t value = (packed >> 11) & 0x7;
        *weekDay = value == 0x7 ? -1 : value;
    }

    if (monthDay) {
        uint32_t value = (packed >> 14) & 0x3F;
        *monthDay = value == 0x3F ? -1 : value;
    }

    if (month) {
        uint32_t value = (packed >> 20) & 0xF;
        *month = value == 0xF ? -1 : value;
    }

    if (year) {
        uint32_t value = (packed >> 24) & 0x1F;
        *year = value == 0x1F ? -1 : value;
    }

    if (flags) {
        uint32_t value = (packed >> 29) & 0x3;
        *flags = value == 0x3 ? -1 : value;
    }
}

// OFFSET: 0x76CAB0
void WowTime::WowDecodeTime(uint32_t packed, WowTime* time) {
    WowTime::WowDecodeTime(packed, &time->minute, &time->hour, &time->weekDay, &time->monthDay, &time->month, &time->year, &time->flags);
}

// OFFSET: 0x76CAE0
void WowTime::WowDecodeTimeToDbDate(uint32_t packed, DBDATESTRUCT* date) {
    int32_t minute = -1;
    int32_t hour = -1;
    int32_t weekDay;
    int32_t monthDay = -1;
    int32_t month = -1;
    int32_t year = -1;
    int32_t flags;

    WowTime::WowDecodeTime(packed, &minute, &hour, &weekDay, &monthDay, &month, &year, &flags);

    if (date) {
        date->year = year + 2000;
        date->month = month + 1;
        date->day = monthDay + 1;
        date->hour = hour;
        date->minute = minute;
        date->second = 0;
    }
}

// OFFSET: 0x76CD40
char* WowTime::WowGetTimeString(const WowTime* time, char* buffer, int32_t bufferSize) {
    uint32_t packed;
    WowTime::WowEncodeTime(packed, time);

    if (!packed) {
        SStrPrintf(buffer, bufferSize, "Not Set");
        return buffer;
    }

    char yearText[8];
    char monthText[8];
    char dayText[8];
    char weekDayText[8];
    char hourText[8];
    char minuteText[8];

    if (time->year >= 0) {
        SStrPrintf(yearText, 8, "%i", time->year + 2000);
    } else {
        SStrPrintf(yearText, 8, "A");
    }

    if (time->month >= 0) {
        SStrPrintf(monthText, 8, "%i", time->month + 1);
    } else {
        SStrPrintf(monthText, 8, "A");
    }

    if (time->monthDay >= 0) {
        SStrPrintf(dayText, 8, "%i", time->monthDay + 1);
    } else {
        SStrPrintf(dayText, 8, "A");
    }

    if (time->weekDay >= 0) {
        SStrPrintf(weekDayText, 8, s_weekDayNames[time->weekDay]);
    } else {
        SStrPrintf(weekDayText, 8, "Any");
    }

    if (time->hour >= 0) {
        SStrPrintf(hourText, 8, "%i", time->hour);
    } else {
        SStrPrintf(hourText, 8, "A");
    }

    if (time->minute >= 0) {
        SStrPrintf(minuteText, 8, "%2.2i", time->minute);
    } else {
        SStrPrintf(minuteText, 8, "A");
    }

    SStrPrintf(buffer, bufferSize, "%s/%s/%s (%s) %s:%s", monthText, dayText, yearText, weekDayText, hourText, minuteText);
    return buffer;
}

// OFFSET: 0x76C360
int32_t WowTime::GetHourAndMinutes() const {
    if (this->hour < 0)
        return 0;
    if (this->minute < 0)
        return 0;

    return this->minute + 60 * this->hour;
}

// OFFSET: 0x76C380
int32_t WowTime::SetHourAndMinutes(int32_t minuteOfDay) {
    this->minute = minuteOfDay % 60;
    this->hour = minuteOfDay / 60;
    return this->hour;
}

// OFFSET: 0x76C3C0
bool WowTime::SetHourAndMinutes(uint32_t hour, uint32_t minute) {
    if (hour >= 24 || minute >= 60)
        return false;

    this->hour = hour;
    this->minute = minute;
    return true;
}

// OFFSET: 0x76C480
bool WowTime::SetDate(uint32_t month, uint32_t monthDay, uint32_t year) {
    if (month >= 12 || monthDay >= 32)
        return false;

    if (year >= 2000)
        year -= 2000;

    if (year > 31)
        return false;

    this->month = month;
    this->monthDay = monthDay;
    this->year = year;
    return true;
}

// OFFSET: 0x76C1F0
int32_t WowTime::GetDaysSinceEpoch() const {
    if (this->year < 0)
        return 0;
    if (this->month < 0)
        return 0;
    if (this->monthDay < 0)
        return 0;

    struct tm date;
    date.tm_sec = 0;
    date.tm_min = 0;
    date.tm_hour = 0;
    date.tm_wday = 0;
    date.tm_yday = 0;
    date.tm_mon = this->month;
    date.tm_mday = this->monthDay + 1;
    date.tm_year = this->year % 100 + 100;
    date.tm_isdst = -1;

    return mktime(&date) / 86400;
}

// OFFSET: 0x76C280
void WowTime::AddDays(int32_t days, bool keepTimeOfDay) {
    if (this->year < 0)
        return;
    if (this->month < 0)
        return;
    if (this->monthDay < 0)
        return;

    struct tm date;
    date.tm_sec = 0;
    date.tm_min = 0;
    date.tm_hour = 0;
    date.tm_wday = 0;
    date.tm_yday = 0;
    date.tm_year = this->year + 100;
    date.tm_mon = this->month;
    date.tm_mday = this->monthDay + 1;
    date.tm_isdst = -1;

    if (keepTimeOfDay) {
        date.tm_hour = this->hour;
        date.tm_min = this->minute;
    }

    time_t shifted = mktime(&date) + days * 86400;

    // Midnight plus a DST step can land on the previous day, so the date-only
    // path is nudged an hour into the day before it is converted back.
    if (!keepTimeOfDay)
        shifted += 3600;

    LocalTimeSafe(&shifted, &date);

    this->year = date.tm_year - 100;
    this->month = date.tm_mon;
    this->monthDay = date.tm_mday - 1;
    this->weekDay = date.tm_wday;

    if (keepTimeOfDay) {
        this->hour = date.tm_hour;
        this->minute = date.tm_min;
    }
}

// OFFSET: 0x76C3F0
int32_t WowTime::AddMinutes(int32_t minutes) {
    int32_t minuteOfDay = this->GetHourAndMinutes() + minutes;

    if (minuteOfDay / 1440) {
        this->AddDays(minuteOfDay / 1440, false);
        minuteOfDay -= 1440 * (minuteOfDay / 1440);
    }

    if (minuteOfDay < 0) {
        this->AddDays(-1, false);
        minuteOfDay += 1440;
    }

    this->minute = minuteOfDay % 60;
    this->hour = minuteOfDay / 60;
    return this->hour;
}

// OFFSET: 0x76C4C0
int32_t WowTime::AddHolidayDuration(int32_t minutes) {
    WowTime original = *this;

    if (minutes / 1440 > 0)
        this->AddDays(minutes / 1440, true);

    this->AddMinutes(minutes % 1440);

    int32_t expected = (original.minute + minutes % 1440 + 60 * original.hour) % 1440;
    int32_t current = this->GetHourAndMinutes();

    if (expected == current)
        return current;

    // A daylight-saving step shows up as exactly one hour of drift; anything
    // else is treated as a real hour that still has to be added.
    if ((this->GetHourAndMinutes() - expected + 1440) % 1440 != 60)
        return this->AddMinutes(60);

    if (this->GetHourAndMinutes() < 60)
        this->AddDays(-1, false);

    this->minute = expected % 60;
    this->hour = expected / 60;
    return this->hour;
}

// OFFSET: 0x76CF10
bool WowTime::InRange(const WowTime& first, const WowTime& last) const {
    if (first == last || first < last) {
        if ((!(*this == first) && !(*this > first)) || !(*this < last))
            return false;
    } else if (!(*this == first) && !(*this > first) && !(*this < last)) {
        return false;
    }

    return true;
}

// OFFSET: 0x76C5E0
int32_t WowTime::CompareMonth(const WowTime& other) const {
    if (this->month > other.month)
        return 1;
    return (this->month >= other.month) - 1;
}

// OFFSET: 0x76C610
int32_t WowTime::CompareDay(const WowTime& other) const {
    if (this->monthDay > other.monthDay)
        return 1;
    return (this->monthDay >= other.monthDay) - 1;
}

// OFFSET: 0x76C640
int32_t WowTime::CompareWeekDay(const WowTime& other) const {
    if (this->weekDay > other.weekDay)
        return 1;
    return (this->weekDay >= other.weekDay) - 1;
}

// OFFSET: 0x76C670
int32_t WowTime::CompareHour(const WowTime& other) const {
    if (this->hour > other.hour)
        return 1;
    return (this->hour >= other.hour) - 1;
}

// OFFSET: 0x76C6A0
int32_t WowTime::CompareMinute(const WowTime& other) const {
    if (this->minute > other.minute)
        return 1;
    return (this->minute >= other.minute) - 1;
}

// OFFSET: 0x76C6D0
bool WowTime::operator<(const WowTime& other) const {
    if (&other == this)
        return false;

    if (other.year >= 0 && this->year >= 0) {
        if (this->year > other.year)
            return false;
        if (this->year < other.year)
            return true;
    }

    if (other.month >= 0 && this->month >= 0) {
        int32_t result = this->CompareMonth(other);
        if (result)
            return result < 0;
    }

    if (other.monthDay >= 0 && this->monthDay >= 0) {
        int32_t result = this->CompareDay(other);
        if (result)
            return result < 0;
    }

    if (other.weekDay >= 0 && this->weekDay >= 0) {
        int32_t result = this->CompareWeekDay(other);
        if (result)
            return result < 0;
    }

    if (other.hour >= 0 && this->hour >= 0) {
        int32_t result = this->CompareHour(other);
        if (result)
            return result < 0;
    }

    if (other.minute >= 0 && this->minute >= 0) {
        int32_t result = this->CompareMinute(other);
        if (result)
            return result < 0;
    }

    return false;
}

// OFFSET: 0x76C7B0
bool WowTime::operator>(const WowTime& other) const {
    if (&other == this)
        return false;

    if (other.year >= 0 && this->year >= 0) {
        if (this->year > other.year)
            return true;
        if (this->year < other.year)
            return false;
    }

    if (other.month >= 0 && this->month >= 0) {
        int32_t result = this->CompareMonth(other);
        if (result)
            return result > 0;
    }

    if (other.monthDay >= 0 && this->monthDay >= 0) {
        int32_t result = this->CompareDay(other);
        if (result)
            return result > 0;
    }

    if (other.weekDay >= 0 && this->weekDay >= 0) {
        int32_t result = this->CompareWeekDay(other);
        if (result)
            return result > 0;
    }

    if (other.hour >= 0 && this->hour >= 0) {
        int32_t result = this->CompareHour(other);
        if (result)
            return result > 0;
    }

    if (other.minute >= 0 && this->minute >= 0) {
        int32_t result = this->CompareMinute(other);
        if (result)
            return result > 0;
    }

    return false;
}

// OFFSET: 0x76C890
bool WowTime::operator==(const WowTime& other) const {
    if (other.year >= 0 && this->year >= 0 && other.year != this->year)
        return false;

    if (other.month >= 0 && this->month >= 0 && other.month != this->month)
        return false;

    if (other.monthDay >= 0 && this->monthDay >= 0 && other.monthDay != this->monthDay)
        return false;

    if (other.weekDay >= 0 && this->weekDay >= 0 && other.weekDay != this->weekDay)
        return false;

    if (other.hour >= 0 && this->hour >= 0 && other.hour != this->hour)
        return false;

    if (other.minute >= 0 && this->minute >= 0 && other.minute != this->minute)
        return false;

    return true;
}

// OFFSET: 0x76CCC0
bool WowTime::operator<=(const WowTime& other) const {
    return *this == other || *this < other;
}

// OFFSET: 0x76CD00
bool WowTime::operator>=(const WowTime& other) const {
    return *this == other || *this > other;
}

// OFFSET: 0x76CB60
CDataStore& operator>>(CDataStore& data, WowTime& time) {
    uint32_t packed;
    data.Get(packed);
    WowTime::WowDecodeTime(packed, &time);
    return data;
}
