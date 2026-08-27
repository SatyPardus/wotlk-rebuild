#ifndef TIME_WOW_TIME_HPP
#define TIME_WOW_TIME_HPP

#include <cstdint>

class CDataStore;

#pragma pack(push, 1)
struct DBDATESTRUCT {
    int16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};
#pragma pack(pop)

class WowTime {
    public:
    // Member variables
    int32_t minute;
    int32_t hour;
    int32_t weekDay;
    int32_t monthDay;
    int32_t month;
    int32_t year;
    int32_t flags;
    int32_t tzHours;

    // Static functions
    static void WowEncodeTime(uint32_t& packed, int32_t minute, int32_t hour, int32_t weekDay, int32_t monthDay, int32_t month, int32_t year, int32_t flags);
    static void WowEncodeTime(uint32_t& packed, const WowTime* time);
    static void WowDecodeTime(uint32_t packed, int32_t* minute, int32_t* hour, int32_t* weekDay, int32_t* monthDay, int32_t* month, int32_t* year, int32_t* flags);
    static void WowDecodeTime(uint32_t packed, WowTime* time);
    static void WowDecodeTimeToDbDate(uint32_t packed, DBDATESTRUCT* date);
    static char* WowGetTimeString(const WowTime* time, char* buffer, int32_t bufferSize);

    // Member functions
    WowTime();

    int32_t GetHourAndMinutes() const;
    int32_t SetHourAndMinutes(int32_t minuteOfDay);
    bool SetHourAndMinutes(uint32_t hour, uint32_t minute);
    bool SetDate(uint32_t month, uint32_t monthDay, uint32_t year);
    int32_t GetDaysSinceEpoch() const;
    void AddDays(int32_t days, bool keepTimeOfDay);
    int32_t AddMinutes(int32_t minutes);
    int32_t AddHolidayDuration(int32_t minutes);
    bool InRange(const WowTime& first, const WowTime& last) const;

    int32_t CompareMonth(const WowTime& other) const;
    int32_t CompareDay(const WowTime& other) const;
    int32_t CompareWeekDay(const WowTime& other) const;
    int32_t CompareHour(const WowTime& other) const;
    int32_t CompareMinute(const WowTime& other) const;

    bool operator<(const WowTime& other) const;
    bool operator>(const WowTime& other) const;
    bool operator==(const WowTime& other) const;
    bool operator<=(const WowTime& other) const;
    bool operator>=(const WowTime& other) const;
};

CDataStore& operator>>(CDataStore& data, WowTime& time);

#endif
