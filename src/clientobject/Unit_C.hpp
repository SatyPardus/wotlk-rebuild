#ifndef CLIENTOBJECT_UNIT_C_HPP
#define CLIENTOBJECT_UNIT_C_HPP

#include <cstdint>
#include "clientobject/CGObject_C.hpp"

class ChrRacesRec;
class ChrClassesRec;

class CGUnit_C : public CGObject_C {
    public:
    CGUnit_C();

    static const char* GetDisplayRaceNameFromRecord(ChrRacesRec* record, uint8_t sexIn, uint8_t* sexOut = nullptr);
    static const char* GetDisplayClassNameFromRecord(ChrClassesRec* record, uint8_t sexIn, uint8_t* sexOut = nullptr);
};

#endif // CLIENTOBJECT_UNIT_C_HPP
