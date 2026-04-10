#include "world/map/CMap.hpp"
#include "world/map/CMapObj.hpp"
#include "world/map/CMapChunk.hpp"
#include "util/SFile.hpp"
#include "world/daynight/DayNight.hpp"
#include <common/ObjectAlloc.hpp>
#include <common/Processor.hpp>
#include <cstring>
#include <storm/Error.hpp>
#include <world/CWorld.hpp>
#include <async/AsyncFileRead.hpp>
#include <gx/Device.hpp>

char CMap::mapPath[STORM_MAX_PATH];
char CMap::mapName[STORM_MAX_PATH];
char CMap::wdtFilename[STORM_MAX_PATH];
uint32_t CMap::s_holeMask[16] = {
    1 << 0, 1 << 1, 1 << 2, 1 << 3,
    1 << 4, 1 << 5, 1 << 6, 1 << 7,
    1 << 8, 1 << 9, 1 << 10, 1 << 11,
    1 << 12, 1 << 13, 1 << 14, 1 << 15,
};
uint32_t CMap::version;
SMMapHeader CMap::header;
SMAreaInfo CMap::areaInfo[64 * 64];
CMapArea* CMap::areaTable[64 * 64];
STORM_EXPLICIT_LIST(CMapBaseObjLink, refLink) CMap::mapAreaList;
int32_t CMap::uniqueId;
int32_t CMap::bDungeon;
int32_t CMap::counts[11];
int32_t CMap::freeCounts[11];
TSGrowableArray<uint32_t> CMap::scCollideList;
uint32_t CMap::scCollideCnt;
uint32_t CMap::cCount;
bool CMap::bPreload;
bool CMap::bIsStreamingMode;

CGxShader* CMap::vertexShader_Terrain[128];
CGxShader* CMap::pixelShader_Terrain0[3];
CGxShader* CMap::pixelShader_Terrain0_env;
CGxShader* CMap::pixelShader_Terrain1[32];
CGxShader* CMap::pixelShader_Terrain2[32];
CGxShader* CMap::pixelShader_Terrain3[96];
CGxShader* CMap::pixelShader_TerrainSM;

uint32_t* CMap::lightHeap;
uint32_t* CMap::cacheLightHeap;
uint32_t* CMap::mapObjGroupHeap;
uint32_t* CMap::mapObjHeap;
uint32_t* CMap::baseObjLinkHeap;
uint32_t* CMap::areaHeap;
uint32_t* CMap::areaMedHeap;
uint32_t* CMap::areaLowHeap;
uint32_t* CMap::chunkHeap;
uint32_t* CMap::doodadDefHeap;
uint32_t* CMap::entityHeap;
uint32_t* CMap::mapObjDefGroupHeap;
uint32_t* CMap::mapObjDefHeap;
uint32_t* CMap::chunkLiquidHeap;

// OFFSET: 0x79E7C0
void CMap::Initialize() {
    //NOP();
    //sub_7C3D90();
    //sub_7AFEE0();
    //sub_7CB990();
    //sub_7B2760();
    //sub_7A03C0();
    memset(&CMap::counts, 0, sizeof(CMap::counts));
    memset(&CMap::freeCounts, 0, sizeof(CMap::freeCounts));
    //memset(&CMap, 0, 0x4000u);
    memset(&CMap::areaTable, 0, sizeof(CMap::areaTable));
    memset(&CMap::areaInfo, 0, sizeof(CMap::areaInfo));
    //if (CMap::scCollideList.m_count < 0x800 && CMap::scCollideList.m_alloc < 0x800) {
    //    m_chunk = CMap::scCollideList.m_chunk;
    //    if (!CMap::scCollideList.m_chunk)
    //        m_chunk = sub_5D0040(&CMap::scCollideList, 0x800u);
    //    v2 = 2048;
    //    if (0x800 % m_chunk)
    //        v2 = m_chunk - 0x800 % m_chunk + 2048;
    //    TSFixedArray::ReallocData(&CMap::scCollideList.m_alloc, v2);
    //}
    CMap::scCollideList.SetCount(2048);
    CMap::scCollideCnt = 0;
    CMap::cCount = 0;
    CMap::uniqueId = -2;
    //s_mapId = -1;
    CMap::bDungeon = 0;
    //CMap::bActive = 0;
    //CMap::oldSelectLightParm = 0;
    //sub_79E3C0();
    memset(CMap::vertexShader_Terrain, 0, sizeof(CMap::vertexShader_Terrain));
    memset(CMap::pixelShader_Terrain0, 0, sizeof(CMap::pixelShader_Terrain0));
    CMap::pixelShader_Terrain0_env = nullptr;
    memset(CMap::pixelShader_Terrain1, 0, sizeof(CMap::pixelShader_Terrain1));
    //CMap::InitializePCFShaders();
    CMap::pixelShader_TerrainSM = nullptr;

    g_theGxDevicePtr->ShaderCreate(CMap::vertexShader_Terrain, GxSh_Vertex, "Shaders\\Vertex", "Terrain", 128);
    g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain0, GxSh_Pixel, "Shaders\\Pixel", "Terrain0", 3);
    g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain0_env, GxSh_Pixel, "Shaders\\Pixel", "Terrain0_env", 1);
    switch (g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel]) {
    case 1:
        g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain1, GxSh_Pixel, "Shaders\\Pixel", "Terrain1", 6);
        g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[8], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w", 6);
        break;

    case 2:
        g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain1, GxSh_Pixel, "Shaders\\Pixel", "Terrain1", 8);
        g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[8], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_1", 1);
        g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[9], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_1", 1);
        g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[10], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_2", 1);
        g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[11], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_2", 1);
        g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[12], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_3", 1);
        g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[13], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_3", 1);
        g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[14], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_4", 1);
        break;

    case 8:
    case 9:
    case 0xA:
        g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain1, GxSh_Pixel, "Shaders\\Pixel", "Terrain1", 4);
        g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[8], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w", 4);
        break;

    default:
        g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain1, GxSh_Pixel, "Shaders\\Pixel", "Terrain1", 32);
        break;
    }

    g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_TerrainSM, GxSh_Pixel, "Shaders\\Pixel", "TerrainSM", 1);
    //    dword_CDFFFC = (int)CGxDevice::PoolCreate(g_theGxDevicePtr, 1, 1, 6144, 0, (int)"CMap::lowDetailIndexPool");
    //    dword_CDFFF8 = (int)CGxDevice::BufCreate(dword_CDFFFC, 2, 3072, 0);
    //    sub_7D58B0(dword_ADFBCC, 0, 1, 16, 545, 0x18u);
    CMap::MapMemInitialize();
    //}
}

// 0x79E4F0
void CMap::InitializePCFShaders() {
    for (int32_t i = 0; i < 32; i++) {
        if (CMap::pixelShader_Terrain2[i]) {
            // g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain2[i]);
        }
    }

    for (int32_t i = 0; i < 96; i++) {
        if (CMap::pixelShader_Terrain3[i]) {
            // g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain3[i]);
        }
    }

    g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain2, GxSh_Pixel, "Shaders\\Pixel", CShaderEffect::s_usePcfFiltering ? "Terrain2_pcf" : "Terrain2", 32);
    g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain3, GxSh_Pixel, "Shaders\\Pixel", CShaderEffect::s_usePcfFiltering ? "Terrain3_pcf" : "Terrain3", 96);
}

void CMap::MapMemInitialize() {
    CMap::lightHeap = NEW(uint32_t);
    *CMap::lightHeap = ObjectAllocAddHeap(212, 128, "WLIGHT", true);

    CMap::cacheLightHeap = NEW(uint32_t);
    *CMap::cacheLightHeap = ObjectAllocAddHeap(132, 256, "WCACHELIGHT", true);

    CMap::mapObjGroupHeap = NEW(uint32_t);
    *CMap::mapObjGroupHeap = ObjectAllocAddHeap(444, 128, "WMAPOBJGROUP", true);

    CMap::mapObjHeap = NEW(uint32_t);
    *CMap::mapObjHeap = ObjectAllocAddHeap(2552, 32, "WMAPOBJ", true);

    CMap::baseObjLinkHeap = NEW(uint32_t);
    *CMap::baseObjLinkHeap = ObjectAllocAddHeap(sizeof(CMapBaseObjLink), 10000, "WBASEOBJLINK", true);

    CMap::areaHeap = NEW(uint32_t);
    *CMap::areaHeap = ObjectAllocAddHeap(sizeof(CMapArea), 16, "WAREA", true);

    CMap::areaMedHeap = NEW(uint32_t);
    *CMap::areaMedHeap = ObjectAllocAddHeap(33404, 16, "WAREAMED", true);

    CMap::areaLowHeap = NEW(uint32_t);
    *CMap::areaLowHeap = ObjectAllocAddHeap(92, 16, "WAREALOW", true);

    CMap::chunkHeap = NEW(uint32_t);
    *CMap::chunkHeap = ObjectAllocAddHeap(sizeof(CMapChunk), 256, "WCHUNK", true);

    CMap::doodadDefHeap = NEW(uint32_t);
    *CMap::doodadDefHeap = ObjectAllocAddHeap(368, 5000, "WDOODADDEF", true);

    CMap::entityHeap = NEW(uint32_t);
    *CMap::entityHeap = ObjectAllocAddHeap(208, 128, "WENTITY", true);

    CMap::mapObjDefGroupHeap = NEW(uint32_t);
    *CMap::mapObjDefGroupHeap = ObjectAllocAddHeap(192, 128, "WMAPOBJDEFGROUP", true);

    CMap::mapObjDefHeap = NEW(uint32_t);
    *CMap::mapObjDefHeap = ObjectAllocAddHeap(344, 64, "WMAPOBJDEF", true);

    CMap::chunkLiquidHeap = NEW(uint32_t);
    *CMap::chunkLiquidHeap = ObjectAllocAddHeap(1092, 64, "WCHUNKLIQUID", true);

    int32_t vendor;
    if (OsGetProcessorFeaturesEx(vendor) & 4) {
        // TODO: dword_CF08F8 = 1;
    }
}

// OFFSET: 0x7BFCE0
void CMap::Load(const char* mapName, int32_t zoneID) {
    // TODO
    // byte_CE049C = 0;
    auto length = SStrCopy(CMap::mapPath, "World\\Maps\\", STORM_MAX_STR);
    SStrCopy(&CMap::mapPath[length], mapName, STORM_MAX_STR);
    SStrCopy(CMap::mapName, mapName, STORM_MAX_STR);
    SStrPrintf(CMap::wdtFilename, 0x100u, "%s\\%s.wdt", CMap::mapPath, CMap::mapName);
    //s_mapLight = CMap::CreateLight(1, 0);
    //CM2Light::SetLightType(&s_mapLight->unk14, 0);
    //CMap::EnableLight(s_mapLight);
    //CMap::UpdateLight(s_mapLight);
    //CMap::PurgeMaps();
    //CMapObj::ClearCache();
    //sub_79FA10();
    //s_mapId = mapid;
    //CMap::bActive = 1;
    CMap::bDungeon = false;
    CMap::bPreload = true;
    CMap::bIsStreamingMode = false; // IsStreamingAndTrial() || SFile::IsStreamingMode();
    // CMap::LoadWdl((int)&dword_CF0900, CMap::mapPath, CMap::mapName);
    CMap::LoadWdt();
    CMap::LoadTextureBlob();
    DayNight::LoadMap(zoneID);
    CMap::PrepareUpdate(false);
    if (!CMap::bIsStreamingMode)
        AsyncFileReadWaitAll();
    if (CWorld::s_loadProgressCallback)
        CWorld::s_loadProgressCallback(1.0, CWorld::s_loadProgressParam);
    CMap::bPreload = false;
    CWorld::s_loadProgressCallback = 0;
    // LODWORD(qword_CD7678) = 1;
    // NOP();
}

void CMap::LoadWdt() {
    SFile* file = nullptr;
    if (!SFile::Open(CMap::wdtFilename, &file) || !file) {
        SErrDisplayAppFatal("CMap::LoadWdt() failed %s\n", CMap::wdtFilename);
    }

    SIffChunk iffChunk = {};

    SFile::Read(file, &iffChunk, sizeof(iffChunk), nullptr, nullptr, nullptr);
    STORM_ASSERT(iffChunk.token == 'MVER' && iffChunk.size == sizeof(CMap::version));
    SFile::Read(file, &CMap::version, sizeof(CMap::version), nullptr, nullptr, nullptr);

    SFile::Read(file, &iffChunk, sizeof(iffChunk), nullptr, nullptr, nullptr);
    STORM_ASSERT(iffChunk.token == 'MPHD' && iffChunk.size == sizeof(CMap::header));
    SFile::Read(file, &CMap::header, sizeof(CMap::header), nullptr, nullptr, nullptr);

    SFile::Read(file, &iffChunk, sizeof(iffChunk), nullptr, nullptr, nullptr);
    STORM_ASSERT(iffChunk.token == 'MAIN' && iffChunk.size == sizeof(CMap::areaInfo));
    SFile::Read(file, &CMap::areaInfo, sizeof(CMap::areaInfo), nullptr, nullptr, nullptr);

    // wdt_uses_global_map_obj
    if (CMap::header.flags & 1) {
        char globalWmoName[256];
        SFile::Read(file, &iffChunk, sizeof(iffChunk), nullptr, nullptr, nullptr);
        STORM_ASSERT(iffChunk.token == 'MWMO' && iffChunk.size <= 256);
        SFile::Read(file, globalWmoName, iffChunk.size, nullptr, nullptr, nullptr);

        SFile::Read(file, &iffChunk, sizeof(iffChunk), nullptr, nullptr, nullptr);
        if (iffChunk.token == 'MODF') {
            SMMapObjDef globalMapObjDef = {};
            SFile::Read(file, &globalMapObjDef, sizeof(globalMapObjDef), nullptr, nullptr, nullptr);
            globalMapObjDef.uniqueId = CMap::uniqueId--;

            // TODO
        }
        CMap::bDungeon = 1;
    }

    if (CMap::header.flags & 2) {
        // TODO: sub_7B7330(2);
    } else {
        // TODO: sub_7B7330(1);
    }

    // sub_7BD8A0();

    SFile::Close(file);
}

void CMap::LoadTextureBlob() {
    char path[STORM_MAX_PATH];
    SStrCopy(path, CMap::wdtFilename, STORM_MAX_STR);
    char* suffix = SStrChrR(path, '.');
    SStrCopy(suffix, ".tex", STORM_MAX_STR);
    // TODO: TextureLoadBlob(path);
}

// OFFSET: 0x7D6980
void CMap::LoadTerrainTexture(CMapArea* area, CMapAreaTexture* areaTexture, int32_t textureId) {
    //v4 = CGxDevice::Caps((char*)g_theGxDevicePtr)->m_texTarget[1] && CGxDevice::Caps((char*)g_theGxDevicePtr)->m_shaderTargets[0] && CGxDevice::Caps((char*)g_theGxDevicePtr)->m_shaderTargets[4];
    //textureFlags = this->textureFlags;
    //LOBYTE(v6) = 0;
    //if (textureFlags)
    //    v6 = textureFlags[a3];
    //v7 = v6 & 1;
    //if (v7 || !byte_CE049D) {
    //    if (!v7 || v4) {
    //        a2->texture = (CTexture*)CMap::LoadTexture(a2->textureName);
    //    } else {
    CImVector color = { 0x00, 0x00, 0x00, 0xFF };
    areaTexture->texture = TextureCreateSolid(color);
    //    }
    //} else {
    //    SStrCopy(v9, a2->textureName, 0x7FFFFFFF);
    //    LastChar = SStr::FindLastChar(v9, 46);
    //    SStrCopy(LastChar, "_s.blp", 0x7FFFFFFF);
    //    a2->texture = (CTexture*)CMap::LoadTexture(v9);
    //}
}

bool CMap::SafeOpen(const char* fileName, SFile** file) {
    int32_t v2 = 10;
    while (!SFile::Open(fileName, file)) {
        //NOP();
        if (!--v2) {
            SErrDisplayAppFatal("CMap::SafeOpen() failed %s", fileName);
        }
    }
    return true;
}

// OFFSET: 0x7C07C0
CMapArea* CMap::AllocArea() {
    uint32_t memHandle;
    void* object = nullptr;

    if (ObjectAlloc(*CMap::areaHeap, &memHandle, &object, 0)) {
        CMapArea* area = new (object) CMapArea();

        area->objectIndex = memHandle;
        // HashTable::AddEntry(&stru_AEED8C, (char *)v1);
        return area;
    }

    // HashTable::AddEntry(&stru_AEED8C, 0);
    return nullptr;
}

// OFFSET: 0x7C0830
CMapChunk* CMap::AllocMapChunk() {
    uint32_t memHandle;
    void* object = nullptr;

    if (ObjectAlloc(*CMap::chunkHeap, &memHandle, &object, 0)) {
        CMapChunk* area = new (object) CMapChunk();

        area->objectIndex = memHandle;
        // HashTable::AddEntry(&CMap::s_mapChunkList, (char *)v1);
        return area;
    }

    // HashTable::AddEntry(&CMap::s_mapChunkList, 0);
    return nullptr;
}

// OFFSET: 0x7C0750
CMapBaseObjLink* CMap::AllocBaseObjLink(CMapBaseObj* baseObj) {
    uint32_t memHandle;
    void* object = nullptr;
    CMapBaseObjLink* link = nullptr;

    if (ObjectAlloc(*CMap::baseObjLinkHeap, &memHandle, &object, 0)) {
        link = new (object) CMapBaseObjLink();

        link->refLink.m_prevlink = nullptr;
        link->refLink.m_next = nullptr;
        link->ownerLink.m_prevlink = nullptr;
        link->ownerLink.m_next = nullptr;
        link->objectIndex = memHandle;
    }

    baseObj->refCount++;
    link->owner = baseObj;
    link->ref = nullptr;

    baseObj->parentLinkList.LinkToTail(link);
    return link;
}

// OFFSET: 0x7C09F0
void CMap::FreeBaseObjLink(CMapBaseObjLink* link) {
    link->ownerLink.Unlink();
    link->refLink.Unlink();
    link->owner->refCount--;
    link->ref = nullptr;
    link->owner = nullptr;

    // This is basically just unlinking again...?
    // Why the double unlinking?
    // sub_6BB6D0(a1);

    ObjectFree(*CMap::baseObjLinkHeap, link->objectIndex);
}

// OFFSET: 0x7D9A70
CMapArea* CMap::PrepareArea(int32_t areaIndexX, int32_t areaIndexY) {
    CMapArea* area = CMap::AllocArea();
    CMapBaseObjLink* link = CMap::AllocBaseObjLink(area);

    CMap::mapAreaList.LinkToTail(link);

    area->tileChunkIndex = { 16 * areaIndexX, 16 * areaIndexY };
    area->index = { areaIndexX, areaIndexY };
    area->unk_C = 0;
    area->topLeft2 = { 17066.666f - area->tileChunkIndex.y * 33.333332f, area->tileChunkIndex.x * -33.333332f + 17066.666f, 0.0f };
    area->bounds.b = { area->topLeft2.x - 533.33331f, area->topLeft2.y - 533.33331f, 0.0f };
    area->bounds.t = { area->topLeft2.x, area->topLeft2.y, 0.0f };

    CMap::areaTable[(64 * areaIndexY) + areaIndexX] = area;
    return area;
}

void CMap::LoadArea(CMapArea* area) {
    char buffer[STORM_MAX_PATH];
    SStrPrintf(buffer, STORM_MAX_PATH, "%s\\%s_%d_%d.adt", CMap::mapPath, CMap::mapName, area->index.x, area->index.y);
    area->Load(buffer);
}

// OFFSET: 0x7B6B00
void CMap::PrepareUpdate(bool a1) {
    // if (HIDWORD(qword_CD7678))
    //     CMap::PurgeMaps();
    // CMap::bspRecurseCount = 0;
    // CMap::mapGetFacetsCount = 0;
    // CMap::oldSelectLightParm = 0;
    // sub_7CF840(flt_CD76A0);
    CMapObj::PrepareUpdate();
    // sub_7B9560();
    CMap::PreUpdateAreas(a1);
    CMap::PrepareMapObjDefs(a1);
    // sub_7B5630();
    // sub_7B5590(a1);
    if (CMap::bPreload) {
        //    if (CMap::s_isStreamingMode) {
        //        v1 = sub_7B4960(&stru_CD7778.X);
        //        v2 = v1;
        //        if (v1) {
        //            v3 = *(int**)(v1 + 112);
        //            if (v3) {
        //                sub_421850(*v3, v11, 260);
        //                AsyncFile::EnterQueueLock();
        //                AsyncFileReadLinkObject(*(_DWORD*)(v2 + 112), 1);
        //                AsyncFile::LeaveQueueLock();
        //                while (*(_DWORD*)(v2 + 112)) {
        //                    v12 = 0i64;
        //                    v13 = 0i64;
        //                    SFile::FileGetIsLocalAmount((int)v11, &v12, &v13);
        //                    AsyncFile::Handler();
        //                    if (CWorld::s_loadProgressCallback) {
        //                        v8 = (double)v12 / (double)v13 * 0.2;
        //                        CWorld::s_loadProgressCallback(LODWORD(v8), CWorld::s_loadProgressParam);
        //                    }
        //                    OsSleep(0xAu);
        //                }
        //            }
        //        }
        //        if (CWorld::s_loadProgressCallback)
        //            CWorld::s_loadProgressCallback(0.2, CWorld::s_loadProgressParam);
        //        CMap::PreUpdateAreas(a1);
        //        CMap::PrepareMapObjDefs(a1);
        //        CMapObj::PrepareUpdate();
        //        sub_7B5630();
        //        AsyncFile::Handler();
        //        v4 = sub_7B49C0(&stru_CD7778);
        //        v14 = 0.0;
        //        v5 = sub_7B5E80(v4, &v14, 0);
        //        if (v5) {
        //            do {
        //                OsSleep(0xAu);
        //                AsyncFile::Handler();
        //                CMap::PreUpdateAreas(a1);
        //                CMap::PrepareMapObjDefs(a1);
        //                CMapObj::PrepareUpdate();
        //                sub_7B5630();
        //                HIDWORD(v13) = sub_7B5E80(v4, &v14, v5);
        //                if (CWorld::s_loadProgressCallback) {
        //                    v9 = v14 * 0.25 + 0.2;
        //                    CWorld::s_loadProgressCallback(LODWORD(v9), CWorld::s_loadProgressParam);
        //                }
        //            } while (HIDWORD(v13));
        //        }
        //        CMap::PreUpdateAreas(a1);
        //        CMap::PrepareMapObjDefs(a1);
        //        CMapObj::PrepareUpdate();
        //        sub_7B5630();
        //        AsyncFile::Handler();
        //        v6 = sub_7B50B0(v4, &v14, 0);
        //        CMapObj::PrepareUpdate();
        //        if (v6) {
        //            do {
        //                OsSleep(0xAu);
        //                AsyncFile::Handler();
        //                CMap::PreUpdateAreas(a1);
        //                CMap::PrepareMapObjDefs(a1);
        //                CMapObj::PrepareUpdate();
        //                sub_7B5630();
        //                v7 = sub_7B50B0(v4, &v14, v6);
        //                HIDWORD(v13) = v7;
        //                if (CWorld::s_loadProgressCallback) {
        //                    HIDWORD(v12) = v6;
        //                    v10 = (double)(v6 - v7) * 0.30000001 / (double)v6 + 0.44999999;
        //                    CWorld::s_loadProgressCallback(LODWORD(v10), CWorld::s_loadProgressParam);
        //                    v7 = HIDWORD(v13);
        //                }
        //            } while (v7);
        //        }
        //        CMap::PreUpdateAreas(a1);
        //        CMap::PrepareMapObjDefs(a1);
        //        CMapObj::PrepareUpdate();
        //        sub_7B5630();
        //        AsyncFile::Handler();
        //    } else {
        AsyncFileReadWaitAll();
        if (CWorld::s_loadProgressCallback)
            CWorld::s_loadProgressCallback(0.25, CWorld::s_loadProgressParam);
        CMap::PreUpdateAreas(a1);
        CMap::PrepareMapObjDefs(a1);
        CMapObj::PrepareUpdate();
        CMap::PrepareMapDoodadDefs();
        AsyncFileReadWaitAll();
        if (CWorld::s_loadProgressCallback)
            CWorld::s_loadProgressCallback(0.5, CWorld::s_loadProgressParam);
        CMap::PreUpdateAreas(a1);
        CMap::PrepareMapObjDefs(a1);
        CMapObj::PrepareUpdate();
        CMap::PrepareMapDoodadDefs();
        AsyncFileReadWaitAll();
        if (CWorld::s_loadProgressCallback)
            CWorld::s_loadProgressCallback(0.66000003, CWorld::s_loadProgressParam);
        CMap::PreUpdateAreas(a1);
        CMap::PrepareMapObjDefs(a1);
        CMapObj::PrepareUpdate();
        CMap::PrepareMapDoodadDefs();
        AsyncFileReadWaitAll();
        if (CWorld::s_loadProgressCallback)
            CWorld::s_loadProgressCallback(0.75, CWorld::s_loadProgressParam);
        //}
    }
}

void CMap::PurgeArea(CMapArea* area) {
    CMap::areaTable[64 * area->index.y + area->index.x] = nullptr;
    // CMapArea::PurgeXXX(area);
    // CMap::FreeArea(area);
}

// OFFSET: 0x7C3730
void CMap::PurgeMaps() {
    for (auto link = CMap::mapAreaList.Head(); link; link = CMap::mapAreaList.Next(link)) {
        CMapArea* area = (CMapArea*)link->owner;
        CMap::FreeBaseObjLink(link);
        CMap::PurgeArea(area);
    }

    // Apparently unused? I see it being used with mapAreaMedHeap, but it never gets created or used
    //for (i = 0; i < 0x4000; i += 4) {
    //    if (*(int*)((char*)&CMap::unk + i)) {
    //        sub_7C0110(*(int*)((char*)&CMap::unk + i));
    //        *(int*)((char*)&CMap::unk + i) = 0;
    //    }
    //}
    
    // WDL related
    //v4 = &dword_CF0900 + 6;
    //v5 = 4096;
    //do {
    //    if (*v4) {
    //        if (*((_DWORD*)*v4 + 21))
    //            sub_7D55B0(dword_ADFBCC, *((_DWORD**)*v4 + 21));
    //        *((_DWORD*)*v4 + 21) = 0;
    //    }
    //    ++v4;
    //    --v5;
    //} while (v5);
}

float sub_7B4830(CAaBox* box, C2Vector* point) {
    float clampedX;
    float diffX;
    float diffY;

    if (box->b.x > point->x)
        clampedX = box->b.x;
    else if (box->t.x >= point->x)
        clampedX = point->x;
    else
        clampedX = box->t.x;

    diffX = clampedX - point->x;

    if (box->b.y > point->y)
        diffY = box->b.y - point->y;
    else if (box->t.y >= point->y)
        diffY = point->y - point->y;
    else
        diffY = box->t.y - point->y;

    return diffX * diffX + diffY * diffY;
}

int32_t sub_7B47F0(const void* aa, const void* bb) {
    float distA = static_cast<const CMapAreaEntry*>(aa)->dist;
    float distB = static_cast<const CMapAreaEntry*>(bb)->dist;

    if (distA < distB)
        return -1;
    if (distA > distB)
        return 1;
    return 0;
}

// OFFSET: 0x7B5950
void CMap::PreUpdateAreas(bool a1) {
    int32_t cellXMin = CWorld::s_chunkRectLow.minX >> 4;
    int32_t cellXMax = CWorld::s_chunkRectLow.maxX >> 4;
    int32_t cellYMin = CWorld::s_chunkRectLow.minY >> 4;
    int32_t cellYMax = CWorld::s_chunkRectLow.maxY >> 4;

    C2Vector worldPos = { CWorld::s_currentWorldPos.x, CWorld::s_currentWorldPos.y };

    size_t numAreas = 0;
    void* stackMem = alloca(8192);
    CMapAreaEntry* areas = (CMapAreaEntry*)stackMem;

    //sub_7B53B0();
    //sub_7B5420();
    //sub_7B5500();
    //sub_7B54A0();

    bool shouldWaitForAsync = !(CMap::bIsStreamingMode || CMap::bPreload);

    for (auto link = CMap::mapAreaList.Head(); link; link = CMap::mapAreaList.Next(link)) {
        CMapArea* area = static_cast<CMapArea*>(link->owner);

        if (area->index.x >= cellXMin && area->index.x <= cellXMax &&
            area->index.y >= cellYMin && area->index.y <= cellYMax) {
            areas[numAreas].dist = sub_7B4830(&area->bounds, &worldPos);
            areas[numAreas].area = area;
            numAreas++;
        } else if (!area->asyncObject || !area->asyncObject->isCurrent) {
            CMap::FreeBaseObjLink(link);
            CMap::PurgeArea(area);
        }
    }

    for (int32_t cellX = cellXMin; cellX <= cellXMax; cellX++) {
        for (int32_t cellY = cellYMin; cellY <= cellYMax; cellY++) {
            int32_t cellIndex = (cellY * 64) + cellX;
            SMAreaInfo info = CMap::areaInfo[cellIndex];
            CMapArea* area = CMap::areaTable[cellIndex];

            if ((info.flags & 1) != 0 && !area) {
                area = CMap::PrepareArea(cellX, cellY);
                areas[numAreas].area = area;
                areas[numAreas].dist = sub_7B4830(&area->bounds, &worldPos);
                numAreas++;
            }
        }
    }

    if (numAreas > 0) {
        CiRect chunkRect = {};

        qsort(areas, numAreas, sizeof(CMapAreaEntry), sub_7B47F0);

        for (int32_t i = 0; i < numAreas; i++) {
            CMapArea* area = areas[i].area;
            float dist = areas[i].dist;

            if (!area->fileBuffer) {
                CMap::LoadArea(area);
            }

            if (area->index.x >= cellXMin && area->index.x <= cellXMax &&
                area->index.y >= cellYMin && area->index.y <= cellYMax) {
                int32_t tileXMin = 16 * area->index.x;
                int32_t tileYMin = 16 * area->index.y;
                int32_t tileXMax = 16 * area->index.x + 15;
                int32_t tileYMax = 16 * area->index.y + 15;
                chunkRect.minX = tileXMin;
                chunkRect.minY = tileYMin;
                chunkRect.maxX = tileXMax;
                chunkRect.maxY = tileYMax;

                if (tileXMin <= CWorld::s_chunkRectHigh.maxX &&
                    tileYMin <= CWorld::s_chunkRectHigh.maxY &&
                    tileXMax >= CWorld::s_chunkRectHigh.minX &&
                    tileYMax >= CWorld::s_chunkRectHigh.minY &&
                    shouldWaitForAsync &&
                    area->asyncObject) {
                    AsyncFileReadWait(area->asyncObject);
                }

                if (area->asyncObject) {
                    //if (CMap::s_isStreamingMode && Base[LODWORD(v23)].dist < 71111.109)
                    //    v43.y = v23;
                } else {
                    chunkRect.minX = area->tileChunkIndex.x;
                    chunkRect.minY = area->tileChunkIndex.y;
                    chunkRect.maxX = area->tileChunkIndex.x + 15;
                    chunkRect.maxY = area->tileChunkIndex.y + 15;
                    CMap::UpdateArea(a1, area, &chunkRect, 0);
                }
            }
        }
    }

    //if (CMap::s_isStreamingMode) {
    //    AsyncFile::EnterQueueLock();
    //    v31 = v46.y;
    //    if (v46.y >= 0.0) {
    //        p_dist = &Base[LODWORD(v46.y)].dist;
    //        do {
    //            v33 = *((_DWORD*)p_dist - 1);
    //            if (*(_DWORD*)(v33 + 112) && (*p_dist < 1111.1111 || *p_dist < 71111.109 && !CWorldView::FrustumCull(v33 + 36))) {
    //                AsyncFileReadLinkObject(*(CAsyncObject**)(v33 + 112), 1);
    //            }
    //            --LODWORD(v31);
    //            p_dist -= 2;
    //        } while (v31 >= 0.0);
    //    }
    //    AsyncFile::LeaveQueueLock();
    //}

    if (a1) {
        // CMap::UpdateBarriers();
    }
}

// OFFSET: 0x7B4DF0
void CMap::UpdateArea(bool a1, CMapArea* area, CiRect* chunkRect, int32_t depth) {
    if (chunkRect->minX > CWorld::s_chunkRectLow.maxX ||
        chunkRect->minY > CWorld::s_chunkRectLow.maxY ||
        chunkRect->maxX < CWorld::s_chunkRectLow.minX ||
        chunkRect->maxY < CWorld::s_chunkRectLow.minY) {
        area->PurgeChunks(chunkRect);
        return;
    }

    if (depth == 2) {
        area->Update(a1, chunkRect);
        return;
    }

    int32_t midX = chunkRect->minX + ((chunkRect->maxX - chunkRect->minX) >> 1);
    int32_t midY = chunkRect->minY + ((chunkRect->maxY - chunkRect->minY) >> 1);

    CiRect quad;

    quad.minY = chunkRect->minY;
    quad.minX = chunkRect->minX;
    quad.maxY = midY;
    quad.maxX = midX;
    CMap::UpdateArea(a1, area, &quad, depth + 1);

    quad.minY = chunkRect->minY;
    quad.minX = midX + 1;
    quad.maxY = midY;
    quad.maxX = chunkRect->maxX;
    CMap::UpdateArea(a1, area, &quad, depth + 1);

    quad.minY = midY + 1;
    quad.minX = midX + 1;
    quad.maxY = chunkRect->maxY;
    quad.maxX = chunkRect->maxX;
    CMap::UpdateArea(a1, area, &quad, depth + 1);

    quad.minY = midY + 1;
    quad.minX = chunkRect->minX;
    quad.maxY = chunkRect->maxY;
    quad.maxX = midX;
    CMap::UpdateArea(a1, area, &quad, depth + 1);
}

// OFFSET: 0x7B6110
void CMap::PrepareMapObjDefs(bool a1) {
    // TODO
}

// OFFSET: 0x7B5630
void CMap::PrepareMapDoodadDefs() {
    // TODO
}
