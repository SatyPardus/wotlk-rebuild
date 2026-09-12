#include "clientobject/PlayerName.hpp"
#include "ui/FrameScript.hpp"
#include <gx/Font.hpp>
#include "util/CStatus.hpp"
#include "gx/Texture.hpp"
#include "common/DataAllocator.hpp"
#include <console/Console.hpp>
#include "clientobject/ObjectMgrClient.hpp"
#include "clientobject/CGObject_C.hpp"
#include <ui/Util.hpp>
#include "console/CVar.hpp"
#include <world/CWorldScene.hpp>

static CGxFont* s_playerNameFont;
static CGxStringBatch* s_playerNameBatch;
static HTEXTURE s_playerNameIcons;
static CDataAllocator s_freePlayerNames(0x38, 0x100);
static uint32_t s_nameMask;
static uint32_t s_lastRenderFrame;
static STORM_EXPLICIT_LIST(PLAYERNAMEDESC, m_link) s_playerNames;

// OFFSET: 0x7E64D0
void PlayerNameInitialize() {
    PlayerNameShutdown();
    const char* fontName;
    if (FrameScript_GetVariable("UNIT_NAME_FONT", &fontName))
        GxuFontCreateFont(fontName, 0.99f, s_playerNameFont, 4);
    s_playerNameBatch = GxuFontCreateBatch(1, 1);

    CStatus status = {};
    auto flags = CGxTexFlags::CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1);
    s_playerNameIcons = TextureCreate("Interface\\TargetingFrame\\UI-RaidTargetingIcons", flags, &status, 0);
    //CStatus::Destroy(status);
}

// OFFSET: 0x7E53A0
void PlayerNameShutdown() {
    if (s_playerNameFont)
        GxuFontDestroyFont(s_playerNameFont);
    s_playerNameFont = nullptr;
    GxuFontDestroyBatch(s_playerNameBatch);
    s_playerNameBatch = nullptr;
    s_freePlayerNames.Clear(".?AVPLAYERNAMEDESC@@", -2);
    ConsoleCommandUnregister("PlayerNames");
    if (s_playerNameIcons) {
        HandleClose(s_playerNameIcons);
        s_playerNameIcons = nullptr;
    }
}

// OFFSET: 0x7E60E0
bool ObjectNameShowCallback(CVar* cvar, const char* a2, const char* a3, void* param) {
    int32_t v4 = SStrToInt(a3);
    uint32_t prevMask = s_nameMask;
    if (v4)
        s_nameMask |= reinterpret_cast<uint32_t>(param);
    else
        s_nameMask &= ~reinterpret_cast<uint32_t>(param);

    if (prevMask == s_nameMask)
        return true;

    for (PLAYERNAMEDESC* i = s_playerNames.Head(); i; i = s_playerNames.Next(i)) {
        i->m_flags |= 1;
    }

    return true;
}

// OFFSET: 0x7E6150
void PlayerNameRegisterCVars() {
    CVar::Register("UnitNameOwn", 0, 16, "0", ObjectNameShowCallback, 4, 0, reinterpret_cast<void*>(1), 0);
    CVar::Register("UnitNameNPC", 0, 16, "0", ObjectNameShowCallback, 4, 0, reinterpret_cast<void*>(2), 0);
    CVar::Register("UnitNamePlayerGuild", 0, 16, "1", ObjectNameShowCallback, 4, 0, reinterpret_cast<void*>(4), 0);
    CVar::Register("UnitNamePlayerPVPTitle", 0, 16, "1", ObjectNameShowCallback, 4, 0, reinterpret_cast<void*>(8), 0);
    CVar::Register("UnitNameEnemyPlayerName", 0, 16, "1", ObjectNameShowCallback, 4, 0, reinterpret_cast<void*>(0x10), 0);
    CVar::Register("UnitNameEnemyPetName", 0, 16, "1", ObjectNameShowCallback, 4, 0, reinterpret_cast<void*>(0x20), 0);
    CVar::Register("UnitNameEnemyGuardianName", 0, 16, "0", ObjectNameShowCallback, 4, 0, reinterpret_cast<void*>(0x1000), 0);
    CVar::Register("UnitNameEnemyTotemName", 0, 16, "0", ObjectNameShowCallback, 4, 0, reinterpret_cast<void*>(0x40), 0);
    CVar::Register("UnitNameFriendlyPlayerName", 0, 16, "1", ObjectNameShowCallback, 4, 0, reinterpret_cast<void*>(0x80), 0);
    CVar::Register("UnitNameFriendlyPetName", 0, 16, "1", ObjectNameShowCallback, 4, 0, reinterpret_cast<void*>(0x100), 0);
    CVar::Register("UnitNameFriendlyGuardianName", 0, 16, "0", ObjectNameShowCallback, 4, 0, reinterpret_cast<void*>(0x2000), 0);
    CVar::Register("UnitNameFriendlyTotemName", 0, 16, "0", ObjectNameShowCallback, 4, 0, reinterpret_cast<void*>(0x200), 0);
    CVar::Register("UnitNameNonCombatCreatureName", 0, 16, "0", ObjectNameShowCallback, 4, 0, reinterpret_cast<void*>(0x400), 0);
}

// OFFSET: 0x7E5420
float PlayerNameComputeScale(CGObject_C* obj) {
    float height = 0.0f;
    if ((obj->m_obj->m_type & TYPEMASK_UNIT) != 0) {
        height = obj->AsUnit()->GetStandHeight();
    } else {
        //m_worldModel = a1->m_worldModel;
        //if (m_worldModel && CM2Model::IsLoaded(m_worldModel, 0, 0) && CM2Model::HasAttachment(a1->m_worldModel, 18u)) {
        //    v3 = a1->m_worldModel;
        //    p_z = &maybe_CM2Model__GetAttachmentPosition(v3, &v7, 18u)->z;
        //    StandHeight = *p_z - maybe_GetModelWorldTransform(v3, &v6)->z;
        //} else {
        height = obj->m_height * obj->m_scale * 1.25;
        //}
    }
    if (height <= 4.0f)
        return 1.0f;
    return height * 0.25f * 1.5f;
}

// OFFSET: 0x7E5F60
PLAYERNAMEDESC* PlayerNameCreate(WGUID guid) {
    if (guid == 0)
        return nullptr;

    auto obj = ClntObjMgrObjectPtr<CGObject_C*>(guid, TYPEMASK_OBJECT);
    if (!obj || !obj->m_worldModel)
        return nullptr;

    auto name = PLAYERNAMEDESC::Allocate(s_freePlayerNames, 0);
    name->m_guid = guid;
    s_playerNames.LinkToTail(name);
    return name;
}

// OFFSET: 0x7E5130
void PlayerNameTriggerNameRegenerate(PLAYERNAMEDESC* name) {
    if (name)
        name->m_flags |= 1;
}

void PlayerNameTestRender() {
    for (PLAYERNAMEDESC* i = s_playerNames.Head(); i; i = s_playerNames.Next(i)) {
        i->Render();
    }
}

// OFFSET: 0x7E55F0
PLAYERNAMEDESC* PLAYERNAMEDESC::Allocate(CDataAllocator& allocator, uint32_t a2) {
    auto m = ALLOCATOR_GET(allocator);
    PLAYERNAMEDESC* name = new (m) PLAYERNAMEDESC();

    //name->unk_0000 = 0;
    //name->unk_0004 = 0;
    //name->unk_0008 = 0;
    name->m_color = { 0x00, 0x00, 0x00, 0x00 };
    name->m_zOffset = 0.0;
    name->m_guid.guid_low = 0;
    name->m_guid.guid_high = 0;
    name->m_flags = 3;
    name->m_renderFrame = s_lastRenderFrame;
    //name->m_worldText[0] = 0;
    //name->m_worldText[1] = 0;
    //name->m_worldText[2] = 0;
    //name->m_worldText[3] = 0;

    return name;
}

// OFFSET: 0x7E5640
void PLAYERNAMEDESC::Render() {
    if (!this->m_guid) {
        return;
    }

    CGObject_C* object = ClntObjMgrObjectPtr<CGObject_C*>(this->m_guid, TYPEMASK_OBJECT);
    if (!object) {
        return;
    }

    int32_t show = object->ShouldRenderObjectName(s_nameMask);
    float scale = PlayerNameComputeScale(object) * 0.2f;

    C3Vector position;
    object->GetNamePosition(position);

    if (!show) {
        if (this->m_string) {
            GxuFontDestroyString(this->m_string);
        }
    } else {
        if ((this->m_flags & 2) != 0) {
            this->m_flags &= ~2u;
            object->GetSelectionHighlightColor(this->m_color);

            if (this->m_string && (this->m_flags & 1) == 0) {
                GxuFontSetStringColor(this->m_string, this->m_color);
            }
        }

        if (!this->m_string || (this->m_flags & 1) != 0) {
            if (this->m_string) {
                GxuFontDestroyString(this->m_string);
                this->m_zOffset = 0.0f;
                this->m_string = nullptr;
            }

            char text[1024];
            memset(text, 0, sizeof(text));

            int32_t lines = object->UpdateObjectNameString(s_nameMask, text, sizeof(text));
            this->m_zOffset = lines * scale;

            if (s_playerNameFont && text[0]) {
                C3Vector origin = { 0.0f, 0.0f, 1.0f };
                GxuFontCreateString(s_playerNameFont, LanguageProcess(text), scale, origin, 100000.0f, 100000.0f, 0.0f, this->m_string, GxVJ_Bottom, GxHJ_Center, 200, this->m_color, 0.0f, 1.0f);
            }

            this->m_flags &= ~1u;
        }

        if (this->m_string) {
            GxuFontAddToBatch(s_playerNameBatch, this->m_string);
            position.z += this->m_zOffset;
            //####TESTING otherwise not visible
            position.x -= CWorldScene::s_activeWorldView.x;
            position.y -= CWorldScene::s_activeWorldView.y;
            position.z -= CWorldScene::s_activeWorldView.z;
            //#########
            GxuFontSetStringPosition(this->m_string, position);
            GxuFontRenderBatch(s_playerNameBatch);
        }
    }

    //uint32_t raidIndex = GetRaidTargetIndexFromGuid(this->m_guid.guid_low, this->m_guid.guid_high);
    //if (!object->ShouldRenderRaidTargetIcon(raidIndex)) {
    //    return;
    //}
    //
    //CGxTex* texture = TextureGetGxTex(s_raidIconTexture, 0, nullptr);
    //if (!texture) {
    //    return;
    //}
    //
    //if (show) {
    //    position.z += scale;
    //}
    //
    //C44Matrix view;
    //GxXformView(view);
    //
    //g_theGxDevicePtr->RsPush();
    //GxRsSet(GxRs_Lighting, 0);
    //GxRsSet(GxRs_Fog, 0);
    //GxRsSet(GxRs_Culling, 0);
    //GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    //GxRsSetAlphaRef();
    //GxRsSet(GxRs_DepthTest, 0);
    //GxRsSet(GxRs_DepthWrite, 0);
    //
    //uint8_t alpha = PlayerNameComputeDistanceFade(position);
    //
    //float u = (raidIndex & 3) * 0.25f;
    //float v = (raidIndex >> 2) * 0.25f;
    //
    //static const C3Vector s_corner[4] = {
    //    { -0.5f, 1.0f, 0.0f },
    //    { 0.5f, 1.0f, 0.0f },
    //    { 0.5f, 0.0f, 0.0f },
    //    { -0.5f, 0.0f, 0.0f }
    //};
    //
    //static const C2Vector s_uv[4] = {
    //    { 0.00f, 0.00f },
    //    { 0.25f, 0.00f },
    //    { 0.25f, 0.25f },
    //    { 0.00f, 0.25f }
    //};
    //
    //CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPCT), 4);
    //CGxVertexPCT* vertices = reinterpret_cast<CGxVertexPCT*>(g_theGxDevicePtr->BufLock(vertexBuf));
    //
    //for (int32_t i = 0; i < 4; i++) {
    //    vertices[i].p = s_corner[i];
    //    vertices[i].c.b = 0xFF;
    //    vertices[i].c.g = 0xFF;
    //    vertices[i].c.r = 0xFF;
    //    vertices[i].c.a = alpha;
    //    vertices[i].tc[0].x = s_uv[i].x + u;
    //    vertices[i].tc[0].y = s_uv[i].y + v;
    //}
    //
    //g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
    //vertexBuf->unk1C = 1;
    //GxPrimVertexPtr(vertexBuf, GxVBF_PCT);
    //
    //CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), 6);
    //uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
    //
    //indices[0] = 0;
    //indices[1] = 1;
    //indices[2] = 3;
    //indices[3] = 3;
    //indices[4] = 1;
    //indices[5] = 2;
    //
    //g_theGxDevicePtr->BufUnlock(indexBuf, 0);
    //indexBuf->unk1C = 1;
    //g_theGxDevicePtr->PrimIndexPtr(indexBuf);
    //
    //GxXformPush(GxXform_World);
    //g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);
    //
    //C44Matrix billboard;
    //billboard.Translate(view.TransformPoint(position));
    //billboard *= view.Inverse(view.Determinant());
    //GxXformSet(GxXform_World, billboard);
    //
    //CGxBatch batch;
    //batch.m_primType = GxPrim_Triangles;
    //batch.m_start = 0;
    //batch.m_count = 6;
    //batch.m_minIndex = 0;
    //batch.m_maxIndex = 3;
    //g_theGxDevicePtr->Draw(&batch, 1);
    //
    //GxXformPop(GxXform_World);
    //g_theGxDevicePtr->RsPop();
}
