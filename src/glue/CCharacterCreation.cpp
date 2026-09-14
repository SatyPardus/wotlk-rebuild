#include "glue/CCharacterCreation.hpp"
#include "componentcore/CCharacterComponent.hpp"
#include "glue/CCharacterSelection.hpp"
#include "glue/CGlueMgr.hpp"
#include "ui/CSimpleModelFFX.hpp"
#include "model/CM2Model.hpp"
#include "model/CM2Shared.hpp"
#include "clientobject/Player_C.hpp"
#include "client/ClientServices.hpp"
#include "db/Db.hpp"
#include "tempest/Random.hpp"

int32_t CCharacterCreation::m_selectedClassID;
int32_t CCharacterCreation::m_existingCharacterIndex;
CHARACTER_PREFERENCES* CCharacterCreation::m_charPreferences[44];
int32_t CCharacterCreation::m_raceIndex;
CSimpleModelFFX* CCharacterCreation::m_charCustomizeFrame;
float CCharacterCreation::m_charFacing;
uint32_t CCharacterCreation::m_prevSkinIndex;
uint32_t CCharacterCreation::m_prevFaceIndex;
uint32_t CCharacterCreation::m_prevHairColorIndex;
uint32_t CCharacterCreation::m_prevHairStyleIndex;
uint32_t CCharacterCreation::m_prevFacialFeatureIndex;
CCharacterComponent* CCharacterCreation::m_character = nullptr;
TSGrowableArray<ChrClassesRec*> CCharacterCreation::m_classes;
TSGrowableArray<int32_t> CCharacterCreation::m_races;

void CCharacterCreation::Initialize() {
    CCharacterCreation::m_charFacing = 0.0;
    int32_t factionSwitch = 0;
    CCharacterCreation::m_charCustomizeFrame = nullptr;
    CCharacterCreation::m_existingCharacterIndex = -1;

    memset(CCharacterCreation::m_charPreferences, 0, sizeof(CCharacterCreation::m_charPreferences));

    int32_t factionSwitch2 = 0;

    bool weirdCondition = false;

    do {
        for (int32_t raceIndex = 0; raceIndex < g_chrRacesDB.GetNumRecords(); ++raceIndex) {
            auto raceRecord = g_chrRacesDB.GetRecordByIndex(raceIndex);
            if (!raceRecord || (raceRecord->m_flags & 1) != 0) {
                continue;
            }

            auto factionTemplateRecord = g_factionTemplateDB.GetRecord(raceRecord->m_factionID);
            if (!factionTemplateRecord) {
                continue;
            }

            for (int32_t factionGroupIndex = 0; factionGroupIndex < g_factionGroupDB.GetNumRecords(); ++factionGroupIndex) {
                auto factionGroupRecord = g_factionGroupDB.GetRecordByIndex(factionGroupIndex);
                if (!factionGroupRecord || factionGroupRecord->m_maskID == 0) {
                    continue;
                }

                if (((1 << factionGroupRecord->m_maskID) & factionTemplateRecord->m_factionGroup) == 0) {
                    continue;
                }

                if (SStrCmpI(factionGroupRecord->m_internalName, factionSwitch == 1 ? "Horde" : "Alliance", STORM_MAX_STR)) {
                    continue;
                }

                CCharacterCreation::m_races.Add(1, &raceRecord->m_ID);

                factionSwitch = factionSwitch2;
            }
        }

        weirdCondition = factionSwitch++ == -1;
        factionSwitch2 = factionSwitch;
    } while (weirdCondition || factionSwitch == 1);
}

// OFFSET: 0x4E1E20
void CCharacterCreation::Shutdown() {
    if (CCharacterCreation::m_character) {
        CCharacterComponent::FreeComponent(CCharacterCreation::m_character);
        CCharacterCreation::m_character = nullptr;
    }

    // TODO

    for (size_t i = 0; i < 44; ++i) {
        DEL(CCharacterCreation::m_charPreferences[i]);
    }
}

void CCharacterCreation::SetCharCustomizeFrame(CSimpleModelFFX* frame) {
    CCharacterCreation::m_charCustomizeFrame = frame;
}

void CCharacterCreation::SetCharCustomizeModel(char const* filename) {
    if (!CCharacterCreation::m_charCustomizeFrame || !filename || !*filename) {
        return;
    }

    auto model = CCharacterCreation::m_charCustomizeFrame->m_model;
    if (model) {
        if (!SStrCmpI(filename, model->m_shared->m_filePath, STORM_MAX_STR)) {
            return;
        }
    }

    CCharacterCreation::m_charCustomizeFrame->SetModel(filename);
    // BYTE1(CCharacterCreation::m_charCustomizeFrame[1].m_onAttributeChange.unk) = 1;

    model = CCharacterCreation::m_charCustomizeFrame->m_model;
    if (!model) {
        return;
    }

    CCharacterCreation::m_charCustomizeFrame->m_lightArray2[0].unk_01BC = 1;
    model->m_lightingCallback = CCharacterSelection::GenericLightingCallback;
    model->m_lightingArg = CCharacterCreation::m_charCustomizeFrame->m_lightArray2;
    // LOBYTE(CCharacterCreation::m_charCustomizeFrame[1].m_onEnable.unk) = 1;
    // TODO: LightingCallback + Particles
    model->IsDrawable(1, 1);

    auto characterModel = CCharacterCreation::m_character->m_data.m_model;
    if (characterModel) {
        characterModel->AttachToParent(model, 0, nullptr, 0);
    }
}

void CCharacterCreation::ResetCharCustomizeInfo() {
    if (!CCharacterCreation::m_charCustomizeFrame) {
        return;
    }

    CCharacterCreation::m_existingCharacterIndex = -1;

    auto model = CCharacterCreation::m_charCustomizeFrame->m_model;
    if (model) {
        model->DetachAllChildrenById(0);
    }

    ComponentData data;
    CCharacterCreation::GetRandomRaceAndSex(&data);
    CCharacterCreation::CalcClasses(data.m_preferences.raceID);
    CCharacterCreation::InitCharacterComponent(&data, 1);

    auto classID = CCharacterCreation::GetRandomClassID();
    CCharacterCreation::SetSelectedClass(classID);

    data.m_preferences.classID = CCharacterCreation::m_selectedClassID;

    CCharacterCreation::m_raceIndex = -1;
    for (uint32_t i = 0; i < CCharacterCreation::m_races.Count(); ++i) {
        if (CCharacterCreation::m_races[i] == CCharacterCreation::m_character->m_data.m_preferences.raceID) {
            CCharacterCreation::m_raceIndex = i;
            break;
        }
    }

    // TODO: CNameGen::LoadNames
    CCharacterCreation::Sub4E6AE0(CCharacterCreation::m_character, 1);
}

// OFFSET: 0x4DFF10
void CCharacterCreation::GetRandomRaceAndSex(ComponentData* data) {
    data->m_preferences.sexID = CRandom::dice(2, g_rndSeed);
    data->m_preferences.raceID = 0;

    ChrRacesRec* raceRec = nullptr;
    uint32_t raceId = 0;
    do {
        if (CCharacterCreation::m_races.Count()) {
            data->m_preferences.raceID = CCharacterCreation::m_races[CRandom::dice(CCharacterCreation::m_races.Count(), g_rndSeed)];
        }

        raceRec = g_chrRacesDB.GetRecord(data->m_preferences.raceID);
    } while (raceRec->m_requiredExpansion > ClientServices::GetInstance()->GetExpansionLevel());
}

void CCharacterCreation::CalcClasses(uint32_t raceID) {
    uint32_t count = 0;

    for (int32_t i = 0; i < g_charBaseInfoDB.GetNumRecords(); ++i) {
        auto record = g_charBaseInfoDB.GetRecordByIndex(i);
        if (record && record->m_raceID == raceID) {
            ++count;
        }
    }

    CCharacterCreation::m_classes.SetCount(count);

    uint32_t index = 0;

    for (int32_t i = 0; i < g_charBaseInfoDB.GetNumRecords(); ++i) {
        auto record = g_charBaseInfoDB.GetRecordByIndex(i);
        if (!record || record->m_raceID != raceID) {
            continue;
        }

        CCharacterCreation::m_classes[index] = g_chrClassesDB.GetRecord(record->m_classID);
        ++index;
    }
}

// OFFSET: 0x4E0FD0
void CCharacterCreation::Dress() {
    if (CCharacterCreation::m_existingCharacterIndex >= 0) {
        CharacterSelectionDisplay* display = CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex);

        if (display) {
            for (uint32_t slot = 0; slot < 23; slot++) {
                uint32_t itemDisplayId = display->m_characterInfo.items[slot].displayID;

                if (!itemDisplayId) {
                    continue;
                }

                bool skip;

                if (display->m_characterInfo.classID == 3) {
                    if (slot == 15) {
                        continue;
                    }

                    skip = slot == 16;
                } else {
                    skip = slot == 17;
                }

                if (skip || slot == 0) {
                    continue;
                }

                if ((display->m_characterInfo.flags & 0x800) && slot == 14) {
                    continue;
                }

                ItemDisplayInfoRec rec;
                ItemDisplayInfoRec* found = g_itemDisplayInfoDB.GetRecord(itemDisplayId);

                if (found) {
                    rec = *found;
                }

                int32_t itemVisualId = display->m_characterInfo.items[slot].auraID;

                if (found && g_itemVisualsDB.GetRecord(rec.m_itemVisual)) {
                    itemVisualId = 0;
                }

                if (slot != 15 && slot != 16 && slot != 17) {
                    CCharacterCreation::m_character->AddItemBySlot(slot, itemDisplayId, itemVisualId);
                } else if (found) {
                    CCharacterCreation::m_character->AddHandItem(CCharacterCreation::m_character->m_data.m_model, &rec, slot, 0, 0, display->m_characterInfo.items[slot].type == 14, 0, itemVisualId);
                }

                if (slot == 18 && found && (rec.m_flags & 1)) {
                    // TODO: GuildGetGuildTabard (0x7EADA0) and ApplyGuildColor (0x4EC1C0)
                    //
                    // int32_t emblemStyle;
                    // int32_t emblemColor;
                    // int32_t borderStyle;
                    // int32_t borderColor;
                    // int32_t backgroundColor;
                    //
                    // if (GuildGetGuildTabard(display->m_characterInfo.guid, display->m_characterInfo.guildID, 0, 0, &emblemStyle, &emblemColor, &borderStyle, &borderColor, &backgroundColor)) {
                    //     CCharacterCreation::m_character->ApplyGuildColor(emblemStyle, emblemColor, borderStyle, borderColor, backgroundColor);
                    // }
                }
            }

            return;
        }
    }

    for (uint32_t i = 0; i < 12; i++) {
        CCharacterCreation::m_character->RemoveItem(static_cast<ITEM_SLOT>(i));
    }

    CM2Model* model = CCharacterCreation::m_character->m_data.m_model;

    CCharacterCreation::m_character->RemoveHandItem(model, 15, 0, 0);
    CCharacterCreation::m_character->RemoveHandItem(model, 16, 0, 0);
    CCharacterCreation::m_character->RemoveHandItem(model, 16, 0, 1);
    CCharacterCreation::m_character->RemoveHandItem(model, 17, 0, 0);

    CharStartOutfitRec* outfit = nullptr;

    for (int32_t i = 0; i < g_charStartOutfitDB.GetNumRecords(); i++) {
        CharStartOutfitRec* row = g_charStartOutfitDB.GetRecordByIndex(i);

        if (row && row->m_raceID == CCharacterCreation::m_character->m_data.m_preferences.raceID && row->m_classID == CCharacterCreation::m_selectedClassID && row->m_sexID == CCharacterCreation::m_character->m_data.m_preferences.sexID) {
            outfit = row;
            break;
        }
    }

    if (!outfit) {
        return;
    }

    for (int32_t i = 0; i < 24; i++) {
        if (outfit->m_displayItemID[i] <= 0) {
            continue;
        }

        int32_t inventoryType = outfit->m_inventoryType[i];

        if (inventoryType == 1) {
            continue;
        }

        if (CCharacterCreation::m_selectedClassID == 3) {
            if (inventoryType == 13 || inventoryType == 17 || inventoryType == 21 || inventoryType == 22) {
                continue;
            }

            if (inventoryType == 15 || inventoryType == 26) {
                ItemDisplayInfoRec* found = g_itemDisplayInfoDB.GetRecord(outfit->m_displayItemID[i]);

                if (found) {
                    CCharacterCreation::m_character->AddHandItem(model, found, 17, 0, 0, 0, inventoryType == 26, 0);

                    // TODO: bow string draw callback (CM2Model::SetLoadedCallback with 0x4E2280)
                    //
                    // if (inventoryType == 15) {
                    //     for (CM2Model* child = model->m_attachList; child; child = child->m_attachNext) {
                    //         if (child->m_attachmentId == attachment) {
                    //             child->SetLoadedCallback(SetupBowStringDraw, nullptr);
                    //             break;
                    //         }
                    //     }
                    // }
                }
            } else if (inventoryType == 18) {
                ItemDisplayInfoRec* found = g_itemDisplayInfoDB.GetRecord(outfit->m_displayItemID[i]);

                if (found && found->m_modelName[0][0]) {
                    CCharacterCreation::m_character->AddItem(ITEMSLOT_11, found, 0);
                }

                continue;
            }
        }

        CCharacterCreation::m_character->AddItemByType(inventoryType, outfit->m_displayItemID[i]);
    }
}

void CCharacterCreation::InitCharacterComponent(ComponentData* data, int32_t randomize) {
    auto record = Player_C_GetModelName(data->m_preferences.raceID, data->m_preferences.sexID);
    if (!record || !record->m_modelName[0]) {
        return;
    }

    if (CCharacterCreation::m_character) {
        auto model = CCharacterCreation::m_character->m_data.m_model;
        if (model->m_attachParent) {
            model->DetachFromParent();
        }
        CCharacterComponent::FreeComponent(CCharacterCreation::m_character);
    }

    CCharacterCreation::m_character = CCharacterComponent::AllocComponent();

    auto scene = CCharacterCreation::m_charCustomizeFrame->GetScene();
    data->m_model = scene->CreateModel(record->m_modelName, 0);
    if (!data->m_model) {
        return;
    }

    // TODO: LightingCallback + particles
    data->m_model->SetBoneSequence(0xFFFFFFFF, 0, 0, 0, 1.0f, 1, 1);

    data->m_flags |= 2u;
    CCharacterCreation::m_character->Init(data, nullptr);

    if (randomize) {
        CCharacterCreation::RandomizeCharFeatures();
    }

    const auto& info = CCharacterCreation::m_character->m_data.m_preferences;
    CCharacterCreation::m_prevSkinIndex = info.skinID;
    CCharacterCreation::m_prevFaceIndex = info.faceID;
    CCharacterCreation::m_prevHairColorIndex = info.hairColorID;
    CCharacterCreation::m_prevHairStyleIndex = info.hairStyleID;
    CCharacterCreation::m_prevFacialFeatureIndex = info.facialHairStyleID;

    CCharacterCreation::SetCharFacing(CCharacterCreation::m_charFacing);
    CCharacterCreation::Dress();

    CCharacterCreation::m_character->RenderPrep(0);

    auto frameModel = CCharacterCreation::m_charCustomizeFrame->m_model;
    if (frameModel) {
        auto model = CCharacterCreation::m_character->m_data.m_model;
        model->AttachToParent(frameModel, 0, nullptr, 0);
    }
}

void CCharacterCreation::RandomizeCharFeatures() {
    CCharacterCreation::m_character->SetRandomSkin(CONTEXT_CHAR_CREATE);
    CCharacterCreation::m_character->SetRandomHairColor(CONTEXT_CHAR_CREATE);
    CCharacterCreation::m_character->SetRandomHairStyle(CONTEXT_CHAR_CREATE);
    CCharacterCreation::m_character->SetRandomFace(CONTEXT_CHAR_CREATE);
    CCharacterCreation::m_character->SetRandomFacialFeature(CONTEXT_CHAR_CREATE);

    const auto& info = CCharacterCreation::m_character->m_data.m_preferences;
    CCharacterCreation::m_prevSkinIndex = info.skinID;
    CCharacterCreation::m_prevFaceIndex = info.faceID;
    CCharacterCreation::m_prevHairColorIndex = info.hairColorID;
    CCharacterCreation::m_prevHairStyleIndex = info.hairStyleID;
    CCharacterCreation::m_prevFacialFeatureIndex = info.facialHairStyleID;
}

void CCharacterCreation::SetSelectedRace(int32_t raceIndex) {
    if (raceIndex < 0 ||
        raceIndex >= CCharacterCreation::m_races.Count() ||
        raceIndex == CCharacterCreation::m_raceIndex) {
        return;
    }

    auto previousRace = CCharacterCreation::m_races[CCharacterCreation::m_raceIndex];
    auto sexID = CCharacterCreation::m_character->m_data.m_preferences.sexID;

    auto preferences = CCharacterCreation::m_charPreferences[2 * previousRace + sexID];
    if (!preferences) {
        preferences = NEW(CHARACTER_PREFERENCES);
        CCharacterCreation::m_charPreferences[2 * previousRace + sexID] = preferences;
    }

    CCharacterCreation::m_character->GetPreferences(preferences);
    CCharacterCreation::m_raceIndex = raceIndex;

    ComponentData data;

    if (CCharacterCreation::m_existingCharacterIndex >= 0) {
        auto display = CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex);
        if (display && display->m_characterInfo.sexID == sexID &&
            (display->m_characterInfo.customizeFlags & 1)) {
            data.m_preferences.raceID = display->m_characterInfo.raceID;
            data.m_preferences.sexID = display->m_characterInfo.sexID;
            data.m_preferences.classID = display->m_characterInfo.classID;
            data.m_preferences.skinID = display->m_characterInfo.skinID;
            data.m_preferences.hairStyleID = display->m_characterInfo.hairStyleID;
            data.m_preferences.hairColorID = display->m_characterInfo.hairColorID;
            data.m_preferences.facialHairStyleID = display->m_characterInfo.facialHairStyleID;
            data.m_preferences.faceID = display->m_characterInfo.faceID;

            CCharacterCreation::InitCharacterComponent(&data, 0);
            CCharacterCreation::SetSelectedSex(display->m_characterInfo.sexID);

            // TODO: CNameGen::LoadNames
            CCharacterCreation::Dress();
            CCharacterCreation::Sub4E6AE0(CCharacterCreation::m_character, 1);
            return;
        }
    }

    auto raceID = CCharacterCreation::m_races[CCharacterCreation::m_raceIndex];
    preferences = CCharacterCreation::m_charPreferences[2 * raceID + sexID];
    if (preferences) {
        data.m_preferences = *preferences;
        CCharacterCreation::CalcClasses(data.m_preferences.raceID);
        if (!CCharacterCreation::IsRaceClassValid(data.m_preferences.raceID, CCharacterCreation::m_selectedClassID)) {
            CCharacterCreation::m_selectedClassID = CCharacterCreation::GetRandomClassID();
        }
        data.m_preferences.classID = CCharacterCreation::m_selectedClassID;
        CCharacterComponent::ValidateComponentData(&data, CONTEXT_CHAR_CREATE);
        CCharacterCreation::InitCharacterComponent(&data, 0);
    } else {
        data.m_preferences.sexID = sexID;
        data.m_preferences.raceID = raceID;
        CCharacterCreation::CalcClasses(data.m_preferences.raceID);
        if (!CCharacterCreation::IsRaceClassValid(data.m_preferences.raceID, CCharacterCreation::m_selectedClassID)) {
            CCharacterCreation::SetSelectedClass(CCharacterCreation::GetRandomClassID());
        }
        data.m_preferences.classID = CCharacterCreation::m_selectedClassID;
        CCharacterCreation::InitCharacterComponent(&data, 1);
    }

    // TODO: CNameGen::LoadNames
    CCharacterCreation::Dress();
    CCharacterCreation::Sub4E6AE0(CCharacterCreation::m_character, 1);
}

void CCharacterCreation::SetSelectedSex(int32_t sexID) {
    if (sexID < 0 || sexID >= 3) {
        return;
    }

    auto previousSex = CCharacterCreation::m_character->m_data.m_preferences.sexID;

    if (sexID == previousSex) {
        return;
    }

    auto raceID = CCharacterCreation::m_races[CCharacterCreation::m_raceIndex];

    auto preferences = CCharacterCreation::m_charPreferences[2 * raceID + previousSex];
    if (!preferences) {
        preferences = NEW(CHARACTER_PREFERENCES);
        CCharacterCreation::m_charPreferences[2 * raceID + previousSex] = preferences;
    }

    CCharacterCreation::m_character->GetPreferences(preferences);

    ComponentData data;
    data.m_preferences.raceID = CCharacterCreation::m_character->m_data.m_preferences.raceID;
    data.m_preferences.sexID = sexID;
    data.m_preferences.classID = CCharacterCreation::m_selectedClassID;

    if (CCharacterCreation::m_existingCharacterIndex >= 0) {
        auto display = CCharacterSelection::GetCharacterDisplay(CCharacterCreation::m_existingCharacterIndex);
        if (display && display->m_characterInfo.sexID == sexID &&
            (display->m_characterInfo.customizeFlags & 1)) {
            data.m_preferences.raceID = display->m_characterInfo.raceID;
            data.m_preferences.sexID = display->m_characterInfo.sexID;
            data.m_preferences.classID = display->m_characterInfo.classID;
            data.m_preferences.skinID = display->m_characterInfo.skinID;
            data.m_preferences.hairStyleID = display->m_characterInfo.hairStyleID;
            data.m_preferences.hairColorID = display->m_characterInfo.hairColorID;
            data.m_preferences.facialHairStyleID = display->m_characterInfo.facialHairStyleID;
            data.m_preferences.faceID = display->m_characterInfo.faceID;

            CCharacterCreation::InitCharacterComponent(&data, 0);

            // TODO: CNameGen::LoadNames
            CCharacterCreation::Sub4E6AE0(CCharacterCreation::m_character, 1);
            return;
        }
    }

    preferences = CCharacterCreation::m_charPreferences[2 * raceID + sexID];
    if (preferences) {
        data.m_preferences = *preferences;
        data.m_preferences.classID = CCharacterCreation::m_selectedClassID;
        CCharacterComponent::ValidateComponentData(&data, CONTEXT_CHAR_CREATE);
        CCharacterCreation::InitCharacterComponent(&data, 0);
    } else {
        data.m_preferences.raceID = CCharacterCreation::m_character->m_data.m_preferences.raceID;
        data.m_preferences.sexID = sexID;
        data.m_preferences.classID = CCharacterCreation::m_selectedClassID;
        CCharacterCreation::InitCharacterComponent(&data, 1);
    }

    // TODO: CNameGen::LoadNames
    CCharacterCreation::Sub4E6AE0(CCharacterCreation::m_character, 1);
}

void CCharacterCreation::SetSelectedClass(int32_t classID) {
    if (!CCharacterCreation::IsClassValid(classID)) {
        return;
    }

    CCharacterCreation::m_selectedClassID = classID;
    ComponentData data;
    data.m_preferences = CCharacterCreation::m_character->m_data.m_preferences;
    data.m_preferences.classID = classID;
    CCharacterComponent::ValidateComponentData(&data, CONTEXT_CHAR_CREATE);
    CCharacterCreation::InitCharacterComponent(&data, 0);
    CCharacterCreation::Dress();

    CCharacterCreation::Sub4E6AE0(CCharacterCreation::m_character, 1);
}

void CCharacterCreation::CycleCharCustomization(CHAR_CUSTOMIZATION_TYPE customization, int32_t delta) {
    switch (customization) {
    case CHAR_CUSTOMIZATION_SKIN:
        if (delta <= 0)
            CCharacterCreation::m_character->SetPrevSkin(CONTEXT_CHAR_CREATE);
        else
            CCharacterCreation::m_character->SetNextSkin(CONTEXT_CHAR_CREATE);
        break;
    case CHAR_CUSTOMIZATION_FACE:
        if (delta <= 0)
            CCharacterCreation::m_character->SetPrevFace(CONTEXT_CHAR_CREATE, CCharacterCreation::m_prevSkinIndex);
        else
            CCharacterCreation::m_character->SetNextFace(CONTEXT_CHAR_CREATE, CCharacterCreation::m_prevSkinIndex);
        break;
    case CHAR_CUSTOMIZATION_HAIR_STYLE:
        if (delta <= 0)
            CCharacterCreation::m_character->SetPrevHairStyle(CONTEXT_CHAR_CREATE);
        else
            CCharacterCreation::m_character->SetNextHairStyle(CONTEXT_CHAR_CREATE);
        break;
    case CHAR_CUSTOMIZATION_HAIR_COLOR:
        if (delta <= 0)
            CCharacterCreation::m_character->SetPrevHairColor(CONTEXT_CHAR_CREATE);
        else
            CCharacterCreation::m_character->SetNextHairColor(CONTEXT_CHAR_CREATE);
        break;
    case CHAR_CUSTOMIZATION_FACIAL_FEATURE:
        if (delta <= 0)
            CCharacterCreation::m_character->SetPrevFacialFeature(CONTEXT_CHAR_CREATE);
        else
            CCharacterCreation::m_character->SetNextFacialFeature(CONTEXT_CHAR_CREATE);
        break;
    }
    CCharacterCreation::Sub4E6AE0(CCharacterCreation::m_character, 1);
}

void CCharacterCreation::RandomizeCharCustomization() {
    CCharacterCreation::RandomizeCharFeatures();
    CCharacterCreation::Dress();
    CCharacterCreation::Sub4E6AE0(CCharacterCreation::m_character, 1);
}

void CCharacterCreation::SetCharFacing(float facing) {
    CCharacterCreation::m_charFacing = facing;
    auto model = CCharacterCreation::m_character->m_data.m_model;
    if (model) {
        model->SetWorldTransform(C3Vector(), facing, 1.0f);
    }
}

void CCharacterCreation::CreateCharacter(const char* name) {
    uint64_t guid = 0;
    CharacterSelectionDisplay* display = nullptr;

    auto index = CCharacterCreation::m_existingCharacterIndex;
    if (index >= 0) {
        display = CCharacterSelection::GetCharacterDisplay(index);
        if (!display) {
            return;
        }

        guid = display->m_characterInfo.guid;

        if (display->m_characterInfo.name) {
            if (SStrCmpI(name, display->m_characterInfo.name, STORM_MAX_STR)) {
                name = display->m_characterInfo.name;
            }
        }
    }

    auto validationResult = ClientServices::CharacterValidateName(name);
    if (validationResult != CHAR_NAME_SUCCESS) {
        auto token = ClientServices::GetErrorToken(validationResult);
        auto text = FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE);
        FrameScript_SignalEvent(3, "%s%s", "OKAY", text);
        return;
    }

    const auto& info = CCharacterCreation::m_character->m_data.m_preferences;

    CHARACTER_CREATE_INFO character;
    SStrCopy(character.name, name, sizeof(character.name));
    character.raceID = info.raceID;
    character.classID = CCharacterCreation::m_selectedClassID;
    character.sexID = info.sexID;
    character.skinID = info.skinID;
    character.hairColorID = info.hairColorID;
    character.hairStyleID = info.hairStyleID;
    character.facialHairStyleID = info.facialHairStyleID;
    character.faceID = info.faceID;
    character.outfitID = 0;

    if (guid && display) {
        // TODO
    } else {
        CGlueMgr::CreateCharacter(&character);
    }
}

void CCharacterCreation::SetToExistingCharacter(uint32_t index) {
    // TODO
}

void CCharacterCreation::Sub4E6AE0(CCharacterComponent* component, int32_t a2) {
    // This method can be an analogue of CGlueLoading::StartLoad

    if (SFile::IsTrial()) {
        // TODO
        return;
    }

    if (component) {
        component->RenderPrep(1);
        component->m_data.m_model->IsDrawable(1, 1);
    }
}

int32_t CCharacterCreation::IsRaceClassValid(int32_t raceID, int32_t classID) {
    for (int32_t i = 0; i < g_charBaseInfoDB.GetNumRecords(); ++i) {
        auto record = g_charBaseInfoDB.GetRecordByIndex(i);
        if (record && record->m_raceID == raceID && record->m_classID == classID) {
            return 1;
        }
    }

    return 0;
}

int32_t CCharacterCreation::IsClassValid(int32_t classID) {
    for (uint32_t i = 0; i < CCharacterCreation::m_classes.Count(); ++i) {
        auto record = CCharacterCreation::m_classes[i];
        if (record && record->m_ID == classID) {
            return 1;
        }
    }

    return 0;
}

int32_t CCharacterCreation::GetRandomClassID() {
    return 1;
}
