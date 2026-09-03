#include "glue/CCharacterSelection.hpp"
#include "model/CM2Model.hpp"
#include "model/CM2Shared.hpp"
#include "ui/CSimpleModelFFX.hpp"
#include "client/ClientServices.hpp"
#include "client/Client.hpp"
#include "console/CVar.hpp"
#include "net/Connection.hpp"
#include "clientobject/Player_C.hpp"
#include "db/Db.hpp"
#include "glue/CGlueMgr.hpp"
#include "componentcore/CCharacterComponent.hpp"

CSimpleModelFFX* CCharacterSelection::m_modelFrame = nullptr;
uint32_t CCharacterSelection::m_characterCount = 0;
float CCharacterSelection::m_charFacing = 0.0f;
uint32_t CCharacterSelection::m_restrictHuman = 0;
uint32_t CCharacterSelection::m_restrictDwarf = 0;
uint32_t CCharacterSelection::m_restrictGnome = 0;
uint32_t CCharacterSelection::m_restrictNightElf = 0;
uint32_t CCharacterSelection::m_restrictDraenei = 0;
uint32_t CCharacterSelection::m_restrictOrc = 0;
uint32_t CCharacterSelection::m_restrictTroll = 0;
uint32_t CCharacterSelection::m_restrictTauren = 0;
uint32_t CCharacterSelection::m_restrictUndead = 0;
uint32_t CCharacterSelection::m_restrictBloodElf = 0;
TSGrowableArray<CharacterSelectionDisplay> CCharacterSelection::s_characterList;
uint32_t CCharacterSelection::m_selectionIndex = 0;


void CCharacterSelection::Initialize() {
    // Empty method
}

// OFFSET: none (inlined)
void CCharacterSelection::RenderPrep() {
    auto index = CCharacterSelection::m_selectionIndex;
    if (index >= CCharacterSelection::GetNumCharacters()) {
        return;
    }

    auto component = CCharacterSelection::s_characterList[index].m_component;
    if (component) {
        component->RenderPrep(0);
    }
}

void CCharacterSelection::SetBackgroundModel(const char* modelPath) {
    if (!CCharacterSelection::m_modelFrame || !modelPath || !*modelPath) {
        return;
    }

    auto model = CCharacterSelection::m_modelFrame->m_model;

    // Check if already set
    if (model && !SStrCmpI(modelPath, model->m_shared->m_filePath, STORM_MAX_STR)) {
        return;
    }

    CCharacterSelection::m_modelFrame->SetModel(modelPath);

    CCharacterSelection::m_modelFrame->m_lightArray1[0].unk_01B5 = 1;

    model = CCharacterSelection::m_modelFrame->m_model;

    if (model) {
        CCharacterSelection::m_modelFrame->m_lightArray1[0].unk_01BC = 1;
        model->m_lightingCallback = CCharacterSelection::GenericLightingCallback;
        model->m_lightingArg = CCharacterSelection::m_modelFrame->m_lightArray1;

        model->IsDrawable(1, 1);
    }
}

// OFFSET: 0x4E3A20
void CCharacterSelection::GenericLightingCallback(CM2Model* model, CM2Lighting* lighting, void* userArg) {
    CharacterSelectionDisplay* character = nullptr;
    if (CCharacterSelection::m_selectionIndex >= 0 && CCharacterSelection::m_selectionIndex < CCharacterSelection::s_characterList.Count())
        character = &CCharacterSelection::s_characterList[CCharacterSelection::m_selectionIndex];

    CM2LightArray* lightArray = reinterpret_cast<CM2LightArray*>(userArg);
    if (!lightArray->unk_01BC && character && (character->m_characterInfo.flags & 0x2000) != 0) {
        lightArray++;
        if (!lightArray->m_hasCustomLight) {
            //ModelWorldTransform = maybe_GetModelWorldTransform(a1, &v20);
            //a3.c.x = ModelWorldTransform->x;
            //a3.c.y = ModelWorldTransform->y;
            //z = ModelWorldTransform->z;
            //a3.r = 0.0;
            //m_scene = a1->m_scene;
            //a3.c.z = z;
            //CM2Lighting::Initialize(a2, m_scene, &a3);
            //Row = ClientDB::GetRow(&bnl_g_lightParamsDB.funcTable2, 3);
            //maybe_CCharacterSelection__LoadLightIntBand(v19, 0, Row, 0);
            //maybe_CCharacterSelection__LoadLightIntBand(&arg8, 0, Row, 1);
            //CM2Light::CM2Light(&v17);
            //CM2Light::SetLightType(&v17, 0);
            //v17.m_ambientColor = *C3Vector::C3Vector(&v20, &arg8);
            //v8 = C3Vector::C3Vector(&v20, v19);
            //v17.m_dirColor.x = v8->x;
            //v17.m_dirColor.y = v8->y;
            //v9 = v8->z;
            //v20.x = 0.0;
            //v20.y = 0.0;
            //v20.z = -1.0;
            //v17.m_dirColor.z = v9;
            //CM2Light::SetDirection(&v17, &v20);
            //CM2Light::SetVisible(&v17, 1u);
            //CM2Lighting::AddLight(a2, &v17);
            //CM2Light::Unlink(&v17);
            return;
        }

        lighting->Reset();
        for (int32_t i = 0; i < lightArray->m_lightCount; i++) {
            lighting->AddLight(&lightArray->m_lights[i]);
        }
    }

    if (lightArray->m_hasCustomLight) {
        lighting->Reset();
        for (int32_t i = 0; i < lightArray->m_lightCount; i++) {
            lighting->AddLight(&lightArray->m_lights[i]);
        }
    }

    if (lightArray->unk_01B5) {
        if ((lightArray->m_modelFrame->m_flags & 1) != 0) {
            C3Vector vec = C3Vector(lightArray->m_modelFrame->m_fogColor);
            lighting->SetFog(vec, lightArray->m_modelFrame->m_fogNear, lightArray->m_modelFrame->m_fogFar);
        }
    }
}

void CCharacterSelection::EnumerateCharactersCallback(CHARACTER_INFO& info, void* param) {
    auto character = CCharacterSelection::s_characterList.New();
    character->m_characterInfo = info;
    // TODO: LoadAddOnEnableState(a1 + 8);
}

void CCharacterSelection::ShowCharacter() {
    auto index = CCharacterSelection::m_selectionIndex;
    if (index < 0 || index >= CCharacterSelection::GetNumCharacters()) {
        return;
    }

    if (CCharacterSelection::m_modelFrame) {
        auto model = CCharacterSelection::m_modelFrame->m_model;
        if (model) {
            model->DetachAllChildrenById(0);
            model->DetachAllChildrenById(1);
        }
    }

    CCharacterSelection::m_charFacing = 0.0;

    auto& character = CCharacterSelection::s_characterList[index];
    if (character.m_component) {
        // TODO: info = DayNightGetInfo();
        float v42;
        if (character.m_characterInfo.flags & 0x2000) {
            // FFX::SetEffect(CGlueMgr__m_deathEffect);
            v42 = 0.15f;
        } else {
            // FFX::SetEffect(CGlueMgr__m_glowEffect);
            v42 = 0.4f;
        }
        // *((float *)info + 75) = v42;

        if (CCharacterSelection::m_modelFrame->m_model) {
            character.m_component->m_data.m_model->AttachToParent(
                CCharacterSelection::m_modelFrame->m_model,
                0,
                nullptr,
                0);

            if (character.m_petModel) {
                character.m_petModel->AttachToParent(
                    CCharacterSelection::m_modelFrame->m_model, 1, nullptr,0);
            }
        }

        // TODO: sub_4E6AE0((int)s_charList.m_data[selectionIndex2].m_component, v5);
        return;
    }

    auto rec = Player_C_GetModelName(character.m_characterInfo.raceID, character.m_characterInfo.sexID);
    if (!rec || !rec->m_modelName) {
        return;
    }

    auto scene = CCharacterSelection::m_modelFrame->GetScene();
    auto model = scene->CreateModel(rec->m_modelName, 0);

    ComponentData componentData(character.m_characterInfo);
    componentData.m_model = model;
    componentData.m_flags |= 2;

    character.m_component = CCharacterComponent::AllocComponent();
    character.m_component->Init(&componentData, 0);

    model->m_lightingCallback = CCharacterSelection::GenericLightingCallback;
    model->m_lightingArg = CCharacterSelection::m_modelFrame->m_lightArray2;

    // TODO: set model ribbon emitters & particles
    model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1);


    // Handle pet model
    // Handle hand items

    ++CCharacterSelection::m_characterCount;

    // DUPLICATE (goto in the OG)
    if (character.m_component) {
        // TODO: info = DayNightGetInfo();
        float v42;
        if (character.m_characterInfo.flags & 0x2000) {
            // FFX::SetEffect(CGlueMgr__m_deathEffect);
            v42 = 0.15f;
        } else {
            // FFX::SetEffect(CGlueMgr__m_glowEffect);
            v42 = 0.4f;
        }
        // *((float *)info + 75) = v42;

        if (CCharacterSelection::m_modelFrame->m_model) {
            character.m_component->m_data.m_model->AttachToParent(
                CCharacterSelection::m_modelFrame->m_model,
                0,
                nullptr,
                0);

            if (character.m_petModel) {
                character.m_petModel->AttachToParent(
                    CCharacterSelection::m_modelFrame->m_model, 1, nullptr, 0);
            }
        }

        // TODO: sub_4E6AE0((int)s_charList.m_data[selectionIndex2].m_component, v5);
        return;
    }
}

void CCharacterSelection::SetCharFacing(float facing) {
    if (!CCharacterSelection::m_characterCount) {
        return;
    }

    CCharacterSelection::m_charFacing = facing;

    if (!CCharacterSelection::GetNumCharacters()) {
        return;
    }

    auto index = CCharacterSelection::m_selectionIndex;
    auto component = CCharacterSelection::s_characterList[index].m_component;
    if (component && component->m_data.m_model) {
        component->m_data.m_model->SetWorldTransform(C3Vector(), facing, 1.0f);
    }
}

// OFFSET: 0x4E38F0
void CharacterSelectionDisplay::Destroy() {
    // TODO LoadingScreenMiniDisable

    if (CCharacterComponent::m_activePlayerComponent) {
        CCharacterComponent::FreeComponent(CCharacterComponent::m_activePlayerComponent);
        CCharacterComponent::m_activePlayerComponent = nullptr;
    }

    // TODO the selected character's component is detached from the list and stashed
    // into CCharacterComponent::m_activePlayerComponent here, which needs
    // RemoveHandItem, RemoveItem, RemoveVisuals, AttachToSceneRecursive and
    // GetDisplayIdFromRaceAndGender

    CharacterSelectionDisplay::FreeComponents(&CCharacterSelection::s_characterList);
    CCharacterSelection::s_characterList.Clear();

    // TODO SetSelectedCharacterInfo(0)
}

// OFFSET: 0x4E3080
void CharacterSelectionDisplay::FreeComponents(TSGrowableArray<CharacterSelectionDisplay>* list) {
    for (uint32_t i = 0; i < list->Count(); i++) {
        auto& display = (*list)[i];

        if (display.m_petModel) {
            display.m_petModel->Release();
        }

        if (display.m_component) {
            CCharacterComponent::FreeComponent(display.m_component);
            display.m_component = nullptr;
        }
    }
}

void CCharacterSelection::ClearCharacterList() {
    CCharacterSelection::s_characterList.Clear();
    if (CCharacterSelection::m_modelFrame) {
        auto model = CCharacterSelection::m_modelFrame->m_model;
        if (model) {
            model->DetachAllChildrenById(0);
            model->DetachAllChildrenById(1);
        }
    }

    CCharacterSelection::m_selectionIndex = 0;
    CCharacterSelection::ShowCharacter();

    FrameScript_SignalEvent(8, "%d", CCharacterSelection::m_selectionIndex + 1);

    if (CCharacterSelection::m_modelFrame) {
        CCharacterSelection::m_modelFrame->SetCameraByIndex(0);
    }

    FrameScript_SignalEvent(7, nullptr);
}

void CCharacterSelection::UpdateCharacterList() {
    // TODO: ClearAddOnEnableState(0);

    CCharacterSelection::s_characterList.SetCount(0);

    CCharacterSelection::m_restrictHuman = 0;
    CCharacterSelection::m_restrictDwarf = 0;
    CCharacterSelection::m_restrictGnome = 0;
    CCharacterSelection::m_restrictNightElf = 0;
    CCharacterSelection::m_restrictDraenei = 0;
    CCharacterSelection::m_restrictOrc = 0;
    CCharacterSelection::m_restrictTroll = 0;
    CCharacterSelection::m_restrictTauren = 0;
    CCharacterSelection::m_restrictUndead = 0;
    CCharacterSelection::m_restrictBloodElf = 0;

    ClientServices::EnumerateCharacters(&CCharacterSelection::EnumerateCharactersCallback, nullptr);

    if (CCharacterSelection::s_characterList.Count()) {
        // TODO: Apply restrictions (m_restrictHuman, etc)
        // TODO: CRealmList::m_preferredCategory = 0;

        int32_t selectionIndex = Client::g_lastCharacterIndex->GetInt();
        if (selectionIndex < 0 || selectionIndex >= CCharacterSelection::s_characterList.Count()) {
            selectionIndex = 0;
        }

        CCharacterSelection::m_selectionIndex = selectionIndex;
        CCharacterSelection::ShowCharacter();

        FrameScript_SignalEvent(8, "%d", CCharacterSelection::m_selectionIndex + 1);
    } else {
        CCharacterSelection::m_selectionIndex = 0;
        CCharacterSelection::ShowCharacter();

        FrameScript_SignalEvent(8, "%d", CCharacterSelection::m_selectionIndex + 1);

        if (CCharacterSelection::m_modelFrame) {
            auto model = CCharacterSelection::m_modelFrame->m_model;
            if (model) {
                model->DetachAllChildrenById(0);
                model->DetachAllChildrenById(1);
            }
        }
    }
    FrameScript_SignalEvent(7, nullptr);
}

void CCharacterSelection::OnGetCharacterList() {
    CCharacterSelection::s_characterList.Clear();
    if (CCharacterSelection::m_modelFrame) {
        auto model = CCharacterSelection::m_modelFrame->m_model;
        if (model) {
            model->DetachAllChildrenById(0);
            model->DetachAllChildrenById(1);
        }
    }
    CGlueMgr::GetCharacterList();
}

uint32_t CCharacterSelection::GetNumCharacters() {
    return CCharacterSelection::s_characterList.Count();
}

CharacterSelectionDisplay* CCharacterSelection::GetCharacterDisplay(uint32_t index) {
    if (index >= CCharacterSelection::s_characterList.Count()) {
        return nullptr;
    } else {
        return CCharacterSelection::s_characterList.Ptr() + index;
    }
}
