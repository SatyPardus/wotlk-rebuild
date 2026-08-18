#include "clientobject/Unit_C.hpp"

#include "db/Db.hpp"

CGUnit_C::CGUnit_C() {

}

CGUnit_C::CGUnit_C(CClientObjCreate& objCreate, uint32_t time)
    : CGObject_C(objCreate, time) {
    
}

// OFFSET: 0x717A20
CreatureModelDataRec* CGUnit_C::GetModelData() {
    uint32_t displayId = 0; //this->m_displayId;
    if (!displayId || this->m_unit->UNIT_FIELD_NATIVEDISPLAYID != this->m_unit->UNIT_FIELD_DISPLAYID)
        displayId = this->m_unit->UNIT_FIELD_DISPLAYID;

    CreatureDisplayInfoRec* creatureDisplayInfo = g_creatureDisplayInfoDB.GetRecord(displayId);
    if (!creatureDisplayInfo) {
        //SysMsgPrintf_0(1, 2, "NOCREATUREDISPLAYIDFOUND|%d", displayId);
        return nullptr;
    }

    CreatureModelDataRec* creatureModelData = g_creatureModelDataDB.GetRecord(creatureDisplayInfo->m_modelID);
    if (!creatureModelData) {
        //SysMsgPrintf_0(1, 16, "INVALIDDISPLAYMODELRECORD|%d|%d", creatureDisplayInfo->m_modelID, creatureDisplayInfo->m_ID);
        return nullptr;
    }

    return creatureModelData;
}

// OFFSET: 0x6E6EF0
void CGUnit_C::GetPosition(C3Vector& pos) {
    // TODO
}

// OFFSET: 0x717B20
bool CGUnit_C::GetModelFileName(const char** fileName) {
    CreatureModelDataRec* creatureModelData = this->GetModelData();
    if (!creatureModelData) {
        *fileName = "Spells\\ErrorCube.mdx";
        return true;
    }

    *fileName = creatureModelData->m_modelName;
    return creatureModelData->m_modelName;
}

const char* CGUnit_C::GetDisplayRaceNameFromRecord(ChrRacesRec* record, uint8_t sexIn, uint8_t* sexOut) {
    if (sexOut) {
        *sexOut = sexIn;
    }
    if (!record) {
        return nullptr;
    }
    if (!sexIn) {
        if (record->m_nameMale[0]) {
            return record->m_nameMale;
        }

        if (record->m_nameFemale[0]) {
            if (sexOut) {
                *sexOut = 1;
            }
            return record->m_nameFemale;
        }

        return record->m_name;
    }

    if (sexIn != 1) {
        return record->m_name;
    }

    if (record->m_nameFemale[0]) {
        return record->m_nameFemale;
    }

    if (!record->m_nameMale[0]) {
        return record->m_name;
    }

    if (sexOut) {
        *sexOut = 0;
    }
    return record->m_nameMale;
}

const char* CGUnit_C::GetDisplayClassNameFromRecord(ChrClassesRec* record, uint8_t sexIn, uint8_t* sexOut) {
    if (sexOut) {
        *sexOut = sexIn;
    }
    if (!record) {
        return nullptr;
    }
    if (!sexIn) {
        if (record->m_nameMale[0]) {
            return record->m_nameMale;
        }

        if (record->m_nameFemale[0]) {
            if (sexOut) {
                *sexOut = 1;
            }
            return record->m_nameFemale;
        }

        return record->m_name;
    }

    if (sexIn != 1) {
        return record->m_name;
    }

    if (record->m_nameFemale[0]) {
        return record->m_nameFemale;
    }

    if (!record->m_nameMale[0]) {
        return record->m_name;
    }

    if (sexOut) {
        *sexOut = 0;
    }
    return record->m_nameMale;
}

// OFFSET: 0x70CBA0
void CGUnit_C::SetStorage(CGUnit_C* obj, uintptr_t descriptorPtr, uintptr_t mirrorPtr) {
    CGObject_C::SetStorage(obj, descriptorPtr, mirrorPtr);
    obj->m_unit = reinterpret_cast<CGUnitData*>(descriptorPtr + CGObject::GetDataSize());
    obj->m_unitMirror = reinterpret_cast<void*>(mirrorPtr + 4 * CGObject::TotalFields());
}
