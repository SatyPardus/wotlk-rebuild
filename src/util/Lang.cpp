#include "util/Lang.hpp"
#include "db/StaticDb.hpp"

const char* GetClassGenderName(ChrClassesRec* rec, bool isFemale, bool* onlyFemale) {
    if (onlyFemale)
        *onlyFemale = isFemale;
    if (!rec)
        return nullptr;
    if (!isFemale) {
        if (*rec->m_nameMale)
            return rec->m_nameMale;
        if (*rec->m_nameFemale) {
            if (onlyFemale)
                *onlyFemale = 1;
            return rec->m_nameFemale;
        }
        return rec->m_name;
    }
    if (isFemale != 1)
        return rec->m_name;
    if (*rec->m_nameFemale)
        return rec->m_nameFemale;
    if (!*rec->m_nameMale)
        return rec->m_name;
    if (onlyFemale)
        *onlyFemale = 0;
    return rec->m_nameMale;
}
