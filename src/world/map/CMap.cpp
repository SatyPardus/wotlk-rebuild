#include "world/map/CMap.hpp"
#include "world/map/CMapObj.hpp"
#include "world/map/CMapChunk.hpp"
#include "util/SFile.hpp"
#include "world/daynight/DayNight.hpp"
#include <common/ObjectAlloc.hpp>
#include <common/Processor.hpp>
#include "client/FrameTime.hpp"
#include <cstring>
#include <storm/Error.hpp>
#include <world/CWorld.hpp>
#include <async/AsyncFileRead.hpp>
#include <gx/Device.hpp>
#include "world/CWorldScene.hpp"
#include "model/Model2.hpp"
#include <world/CWorldMath.hpp>
#include "world/map/CMapObjDefGroup.hpp"

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
STORM_EXPLICIT_LIST(CMapRenderChunk, renderChunkLink) CMap::s_mapRenderChunkFreeList;
STORM_EXPLICIT_LIST(CMapRenderChunk, renderChunkLink) CMap::s_mapRenderChunkUpdateList;
STORM_EXPLICIT_LIST(CMapDoodadDef, doodadDefLink) CMap::doodadDefList;
TSHashTable<CMapDoodadDef, uint32_t> CMap::doodadDefHashtable;
TSHashTable<CMapObjDef, uint32_t> CMap::mapObjDefHashtable;
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

bool CMap::enableVertexShaders;
bool CMap::enablePixelShaders;
bool CMap::enableSpecular;
bool CMap::gTerrainPixelShadersValid;
bool CMap::enableSpecularTerrain;
bool CMap::enableTerrainShaderVertex;
bool CMap::enableChunkBatching;

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
    CMapChunk::Initialize();
    //CMapObjRender::Initialize();
    CMapObjGroup::Initialize();
    //CDetailDoodad::Initialize();
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
    //    CMap::lowDetailIndexPool = (int)CGxDevice::PoolCreate(g_theGxDevicePtr, 1, 1, 6144, 0, (int)"CMap::lowDetailIndexPool");
    //    CMap::lowDetailIndexBuf = (int)CGxDevice::BufCreate(dword_CDFFFC, 2, 3072, 0);
    //    VBBList::Initialize(CMapObjGroup::vertexVBList, 0, 1, 16, 545, 0x18u);
    CMap::MapMemInitialize();
    //}
}

// OFFSET: 0x79E4F0
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

// OFFSET: 0x7BD8A0
void CMap::ValidateShaders() {
    CMap::enableVertexShaders = CWorld::s_enables2 & CWorld::Enables2::Enable_VertexShader;
    CMap::enablePixelShaders = (CWorld::s_enables & CWorld::Enables::Enable_PixelShader) != 0;
    CMap::enableSpecular = true;
    CMap::gTerrainPixelShadersValid = false;
    CMap::enableSpecularTerrain = false;
    CMap::enableTerrainShaderVertex = false;
    CMap::enableChunkBatching = false;

    return; // Disable shader stuff for debugging

    bool v1 = (CMap::header.flags >> 2) & 1;

    if ((CWorld::s_enables & CWorld::Enables::Enable_800000) == 0 || !CMap::enablePixelShaders) {
        CMap::enableSpecular = false;
    }

    if (CMap::enablePixelShaders) {
        CGxShader* shader1 = CMap::GetPixelShader(v1, 1, 0);
        CGxShader* shader2 = CMap::GetPixelShader(v1, 0, 0);
        CMap::gTerrainPixelShadersValid = shader1 && shader1->Valid() && shader2 && shader2->Valid();
    }

    if (CMap::enableSpecular && CMap::gTerrainPixelShadersValid) {
        CGxShader* shader1 = CMap::GetPixelShader(v1, 1, 0);
        CGxShader* shader2 = CMap::GetPixelShader(v1, 0, 0);
        CMap::enableSpecularTerrain = shader1 && shader1->Valid() && shader2 && shader2->Valid();
    }

    if (CMap::enableVertexShaders) {
        CMap::enableTerrainShaderVertex = true;
        if (!CMap::gTerrainPixelShadersValid || !CMap::vertexShader_Terrain[0] || !CMap::vertexShader_Terrain[0]->Valid()) {
            CMap::enableTerrainShaderVertex = false;
        }
    }

    if (CMap::gTerrainPixelShadersValid) {
        CMap::enableChunkBatching = CMap::enableTerrainShaderVertex;
    }
}

// OFFSET: 0x79E4B0
CGxShader* CMap::GetPixelShader(bool a1, bool a2, bool a3) {
    if (!a1)
        return CMap::pixelShader_Terrain0[0];
    if (!a2)
        return CMap::pixelShader_Terrain0[2];
    if (a3)
        return CMap::pixelShader_Terrain0_env;
    return CMap::pixelShader_Terrain0[1];
}

void CMap::MapMemInitialize() {
    CMap::lightHeap = NEW(uint32_t);
    *CMap::lightHeap = ObjectAllocAddHeap(212, 128, "WLIGHT", true);

    CMap::cacheLightHeap = NEW(uint32_t);
    *CMap::cacheLightHeap = ObjectAllocAddHeap(132, 256, "WCACHELIGHT", true);

    CMap::mapObjGroupHeap = NEW(uint32_t);
    *CMap::mapObjGroupHeap = ObjectAllocAddHeap(sizeof(CMapObjGroup), 128, "WMAPOBJGROUP", true);

    CMap::mapObjHeap = NEW(uint32_t);
    *CMap::mapObjHeap = ObjectAllocAddHeap(sizeof(CMapObj), 32, "WMAPOBJ", true);

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
    *CMap::doodadDefHeap = ObjectAllocAddHeap(sizeof(CMapDoodadDef), 5000, "WDOODADDEF", true);

    CMap::entityHeap = NEW(uint32_t);
    *CMap::entityHeap = ObjectAllocAddHeap(208, 128, "WENTITY", true);

    CMap::mapObjDefGroupHeap = NEW(uint32_t);
    *CMap::mapObjDefGroupHeap = ObjectAllocAddHeap(sizeof(CMapObjDefGroup), 128, "WMAPOBJDEFGROUP", true);

    CMap::mapObjDefHeap = NEW(uint32_t);
    *CMap::mapObjDefHeap = ObjectAllocAddHeap(sizeof(CMapObjDef), 64, "WMAPOBJDEF", true);

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
    CMap::LoadTex();
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

    CMap::ValidateShaders();

    SFile::Close(file);
}

// OFFSET: 0x7BD540
void CMap::LoadTex() {
    char path[STORM_MAX_PATH];
    SStrCopy(path, CMap::wdtFilename, STORM_MAX_STR);
    char* suffix = SStrChrR(path, '.');
    SStrCopy(suffix, ".tex", STORM_MAX_STR);
    // TODO: TextureLoadBlob(path);
}

// OFFSET: 0x7D9990
HTEXTURE CMap::LoadTexture(const char* fileName) {
    CStatus status;
    CGxTexFlags texFlags = CGxTexFlags(GxTex_LinearMipLinear, 1, 1, 0, 0, 0, 1);
    HTEXTURE texture = TextureCreate(fileName, texFlags, &status, 0);
    // SysMsgAdd(status);
    //CStatus::Destroy(status);
    return texture;
}

// OFFSET: 0x7D6980
void CMap::LoadTerrainTexture(CMapArea* area, CMapAreaTexture* areaTexture, int32_t textureId) {
    bool v4 = g_theGxDevicePtr->Caps().m_texTarget[1] && g_theGxDevicePtr->Caps().m_shaderTargets[0] && g_theGxDevicePtr->Caps().m_shaderTargets[4];
    int32_t v6 = 0;
    if (area->textureFlags)
        v6 = area->textureFlags[textureId];
    if ((v6 & 1) != 0 || !CMap::enableSpecularTerrain) {
        if ((v6 & 1) == 0 || v4)
            areaTexture->texture = CMap::LoadTexture(areaTexture->textureName);
        else {
            CImVector color = { 0x00, 0x00, 0x00, 0xFF };
            areaTexture->texture = TextureCreateSolid(color);
        }
    } else {
        char path[STORM_MAX_PATH];
        SStrCopy(path, areaTexture->textureName, STORM_MAX_STR);
        char* suffix = SStrChrR(path, '.');
        SStrCopy(suffix, "_s.blp", STORM_MAX_STR);
        areaTexture->texture = CMap::LoadTexture(path);
    }
}

// OFFSET: 0x7BD480
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

        area->m_memHandle = memHandle;
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

        area->m_memHandle = memHandle;
        // HashTable::AddEntry(&CMap::s_mapChunkList, (char *)v1);
        return area;
    }

    // HashTable::AddEntry(&CMap::s_mapChunkList, 0);
    return nullptr;
}

// OFFSET: 0x7C0500
CMapRenderChunk* CMap::AllocRenderChunk() {
    CMapRenderChunk* chunk = s_mapRenderChunkFreeList.Head();
    if (!chunk) {
        chunk = (CMapRenderChunk*)SMemAlloc(sizeof(CMapRenderChunk), ".?AVCMapRenderChunk@@", -2, 8);
        if (!chunk)
            return nullptr;
        s_mapRenderChunkFreeList.LinkToTail(chunk);
    }
    chunk->renderChunkLink.Unlink();
    new (chunk) CMapRenderChunk();
    return chunk;
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

// OFFSET: 0x7C01F0
CMapDoodadDef* CMap::AllocDoodadDef() {
    uint32_t memHandle;
    void* object = nullptr;

    if (ObjectAlloc(*CMap::doodadDefHeap, &memHandle, &object, 0)) {
        CMapDoodadDef* mapDoodadDef = new (object) CMapDoodadDef();

        mapDoodadDef->m_memHandle = memHandle;
        return mapDoodadDef;
    }

    return nullptr;
}

// OFFSET: 0x7C03E0
CMapObjDef* CMap::AllocMapObjDef() {
    uint32_t memHandle;
    void* object = nullptr;

    if (ObjectAlloc(*CMap::mapObjDefHeap, &memHandle, &object, 0)) {
        CMapObjDef* def = new (object) CMapObjDef();

        def->m_memHandle = memHandle;
        return def;
    }

    return nullptr;
}

// OFFSET: 0x7BFF20
CMapObj* CMap::AllocMapObj() {
    uint32_t memHandle;
    void* object = nullptr;

    if (ObjectAlloc(*CMap::mapObjHeap, &memHandle, &object, 0)) {
        CMapObj* def = new (object) CMapObj();

        def->m_memHandle = memHandle;
        return def;
    }

    return nullptr;
}

// OFFSET: 0x7BFFE0
CMapObjGroup* CMap::AllocMapObjGroup() {
    uint32_t memHandle;
    void* object = nullptr;

    if (ObjectAlloc(*CMap::mapObjGroupHeap, &memHandle, &object, 0)) {
        CMapObjGroup* def = new (object) CMapObjGroup();

        def->m_memHandle = memHandle;
        return def;
    }

    return nullptr;
}

// OFFSET: 0x7C0910
CMapObjDefGroup* CMap::AllocMapObjDefGroup() {
    uint32_t memHandle;
    void* object = nullptr;

    if (ObjectAlloc(*CMap::mapObjDefGroupHeap, &memHandle, &object, 0)) {
        CMapObjDefGroup* def = new (object) CMapObjDefGroup();

        def->m_memHandle = memHandle;
        return def;
    }

    return nullptr;
}

// Debug function
void CMapDoodadLightingCallback(CM2Model* model, CM2Lighting* lighting, void* userArg) {
    lighting->AddAmbient({ 1.0f, 1.0f, 1.0f });
    lighting->AddDiffuse({ 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f });
    lighting->AddSpecular({ 0.0f, 0.0f, 0.0f });
}

// OFFSET: 0x7BF460
CMapObjDef* CMap::CreateMapObjDef(char* fileName, SMMapObjDef* objectDef, C3Vector* center, bool cached) {
    constexpr float kDegToRad = 0.017453292f;
    constexpr float kPi = 3.1415927;

    uint32_t v23;
    CMapObjDef* mapObjectDef = CMap::mapObjDefHashtable.Ptr(objectDef->uniqueId, v23);
    if (cached && mapObjectDef)
        return mapObjectDef;

	mapObjectDef = CMap::AllocMapObjDef();
    if (cached)
        CMap::mapObjDefHashtable.Insert(mapObjectDef, objectDef->uniqueId, v23);

	mapObjectDef->position = {
        -objectDef->position.z + center->x,
        -objectDef->position.x + center->y,
        objectDef->position.y + center->z,
    };
    mapObjectDef->flags = 0;
    mapObjectDef->nameId = objectDef->nameId;
    mapObjectDef->doodadSet = objectDef->doodadSet;
    mapObjectDef->nameSet = objectDef->nameSet;
    //v6->unk_148 = 0;
    //v6->unk_14C = 0;
    //v6->unk_150 = 0;
    //LOWORD(v6->unk_154) = 0;
	mapObjectDef->mat = C44Matrix();
    mapObjectDef->mat.d0 = mapObjectDef->position.x;
    mapObjectDef->mat.d1 = mapObjectDef->position.y;
    mapObjectDef->mat.d2 = mapObjectDef->position.z;
    mapObjectDef->mat.RotateAroundZ(objectDef->rotation.y * kDegToRad + kPi);
    mapObjectDef->mat.RotateAroundY(objectDef->rotation.x * kDegToRad);
    mapObjectDef->mat.RotateAroundX(objectDef->rotation.z * kDegToRad);
    mapObjectDef->invMat = mapObjectDef->mat.AffineInverse();
    mapObjectDef->bbox.b = {
        center->x + -objectDef->extents.t.z,
        center->y + -objectDef->extents.t.x,
        center->z + objectDef->extents.b.y
    };
    mapObjectDef->bbox.t = {
        center->x + -objectDef->extents.b.z,
        center->y + -objectDef->extents.b.x,
        center->z + objectDef->extents.t.y
    };
    C3Vector half;

    mapObjectDef->sphere.c.x = (mapObjectDef->bbox.b.x + mapObjectDef->bbox.t.x) * 0.5f;
    mapObjectDef->sphere.c.y = (mapObjectDef->bbox.b.y + mapObjectDef->bbox.t.y) * 0.5f;
    mapObjectDef->sphere.c.z = (mapObjectDef->bbox.b.z + mapObjectDef->bbox.t.z) * 0.5f;
    half.x = mapObjectDef->bbox.t.x - mapObjectDef->sphere.c.x;
    half.y = mapObjectDef->bbox.t.y - mapObjectDef->sphere.c.y;
    half.z = mapObjectDef->bbox.t.z - mapObjectDef->sphere.c.z;
    mapObjectDef->sphere.r = sqrtf(half.x * half.x +
                          half.y * half.y +
                          half.z * half.z);
	//mapObjectDef->TSGrowableArray__m_count = 0;
    mapObjectDef->owner = CMapObj::Create(fileName);
    return mapObjectDef;
}

// OFFSET: 0x7BECD0
CMapDoodadDef* CMap::CreateDoodadDef(char* fileName, SMDoodadDef* doodadDef, C3Vector* position) {
    constexpr float kDegToRad = 0.017453292f;
    constexpr float kPi = 3.1415927;

    uint32_t v23;
    CMapDoodadDef* mapDoodadDef = CMap::doodadDefHashtable.Ptr(doodadDef->uniqueId, v23);
    if (mapDoodadDef)
        return mapDoodadDef;

    mapDoodadDef = CMap::AllocDoodadDef();
    uint32_t a2;
    CMap::doodadDefHashtable.Insert(mapDoodadDef, doodadDef->uniqueId, a2);
    CMap::doodadDefList.LinkToTail(mapDoodadDef);

    mapDoodadDef->position = {
        -doodadDef->position.z + position->x,
        -doodadDef->position.x + position->y,
        doodadDef->position.y + position->z,
    };
    mapDoodadDef->sphere.c = mapDoodadDef->position;
    mapDoodadDef->sphere.r = 0.0f;
    mapDoodadDef->bboxStaticEntity.b = mapDoodadDef->position;
    mapDoodadDef->bboxStaticEntity.t = mapDoodadDef->position;
    mapDoodadDef->scale = doodadDef->scale / 1024.0f;

    mapDoodadDef->flags = MAPOBJ_FLAG_UNPLACED;
    if ((doodadDef->flags & 1) != 0)
        mapDoodadDef->flags = MAPOBJ_FLAG_BIODOME | MAPOBJ_FLAG_UNPLACED;
    mapDoodadDef->model = nullptr;
    mapDoodadDef->mat = C44Matrix();
    mapDoodadDef->mat.d0 = mapDoodadDef->position.x;
    mapDoodadDef->mat.d1 = mapDoodadDef->position.y;
    mapDoodadDef->mat.d2 = mapDoodadDef->position.z;
    mapDoodadDef->mat.RotateAroundZ(doodadDef->rotation.z * kDegToRad + kPi);
    mapDoodadDef->mat.RotateAroundY(doodadDef->rotation.y * kDegToRad);
    mapDoodadDef->mat.RotateAroundX(doodadDef->rotation.x * kDegToRad);
    mapDoodadDef->mat.Scale(mapDoodadDef->scale);
    mapDoodadDef->identity = C44Matrix();

    mapDoodadDef->model = CWorldScene::s_m2Scene->CreateModel(fileName, 32);
    if (mapDoodadDef->model) {
        mapDoodadDef->model->m_flag8000 = 1;
        mapDoodadDef->model->SetWorldTransform(mapDoodadDef->position, 180.0f, 1.0f);
        //CWorldScene::LoadModel(v5->model, COERCE_FLOAT(CMapStaticEntity::ModelEventCallback), *(float *)&v5, 0.0);
        //mapDoodadDef->model->m_lightingCallback = MapStaticEntity::ModelLightingCallback;
        mapDoodadDef->model->m_lightingCallback = CMapDoodadLightingCallback;
        mapDoodadDef->model->m_lightingArg = mapDoodadDef;
        mapDoodadDef->model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    }
    return mapDoodadDef;
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
    area->flags = 0;
    area->topLeft2 = { 17066.666f - area->tileChunkIndex.y * 33.333332f, area->tileChunkIndex.x * -33.333332f + 17066.666f, 0.0f };
    area->bounds.b = { area->topLeft2.x - 533.33331f, area->topLeft2.y - 533.33331f, 0.0f };
    area->bounds.t = { area->topLeft2.x, area->topLeft2.y, 0.0f };

    CMap::areaTable[(64 * areaIndexY) + areaIndexX] = area;
    return area;
}

// OFFSET: 0x7D9A20
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
    CMap::PrepareMapDoodadDefs();
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
    for (auto link = CMap::mapAreaList.Head(); link;) {
        auto next = CMap::mapAreaList.Next(link);
        CMapArea* area = (CMapArea*)link->owner;
        CMap::FreeBaseObjLink(link);
        CMap::PurgeArea(area);
        link = next;
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

    int32_t numAreas = 0;
    void* stackMem = alloca(8192);
    CMapAreaEntry* areas = (CMapAreaEntry*)stackMem;

    //sub_7B53B0();
    //sub_7B5420();
    //sub_7B5500();
    //sub_7B54A0();

    bool shouldWaitForAsync = !(CMap::bIsStreamingMode || CMap::bPreload);

    for (auto link = CMap::mapAreaList.Head(); link;) {
        auto next = CMap::mapAreaList.Next(link);
        CMapArea* area = static_cast<CMapArea*>(link->owner);

        if (area->index.x >= cellXMin && area->index.x <= cellXMax &&
            area->index.y >= cellYMin && area->index.y <= cellYMax) {
            areas[numAreas].dist = area->bounds.DistanceSqXY(worldPos);
            areas[numAreas].area = area;
            numAreas++;
        } else if (!area->asyncObject || !area->asyncObject->isCurrent) {
            CMap::FreeBaseObjLink(link);
            CMap::PurgeArea(area);
        }

        link = next;
    }

    for (int32_t cellX = cellXMin; cellX <= cellXMax; cellX++) {
        for (int32_t cellY = cellYMin; cellY <= cellYMax; cellY++) {
            int32_t cellIndex = (cellY * 64) + cellX;
            SMAreaInfo info = CMap::areaInfo[cellIndex];
            CMapArea* area = CMap::areaTable[cellIndex];

            if ((info.flags & 1) != 0 && !area) {
                area = CMap::PrepareArea(cellX, cellY);
                areas[numAreas].area = area;
                areas[numAreas].dist = area->bounds.DistanceSqXY(worldPos);
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
                }
                else {
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
    bool v23 = true;
    if (CMap::bIsStreamingMode || CMap::bPreload)
        v23 = false;

    for (auto mapObjDef = CMap::mapObjDefHashtable.Head(); mapObjDef;) {
        auto next = CMap::mapObjDefHashtable.Next(mapObjDef);

        if (mapObjDef->bbox.t.x >= CWorld::s_objectAreaOfInterest.b.x
            && mapObjDef->bbox.t.y >= CWorld::s_objectAreaOfInterest.b.y
            && mapObjDef->bbox.t.z >= CWorld::s_objectAreaOfInterest.b.z
            && mapObjDef->bbox.b.x <= CWorld::s_objectAreaOfInterest.t.x
            && mapObjDef->bbox.b.y <= CWorld::s_objectAreaOfInterest.t.y
            && mapObjDef->bbox.b.z <= CWorld::s_objectAreaOfInterest.t.z) {
            if (v23 && !mapObjDef->owner->isGroupLoaded)
                mapObjDef->owner->WaitLoad();
            if ((mapObjDef->flags & 0x80) == 0 && mapObjDef->owner->isGroupLoaded)
                CMap::PrepareMapObjDef(mapObjDef, mapObjDef->owner);
        }
        float dist = mapObjDef->bbox.DistanceSq(CWorld::s_currentWorldPos);
        if (dist < mapObjDef->owner->distToCamera)
            mapObjDef->owner->distToCamera = dist;

        for (auto mapObjDefGroupLink = mapObjDef->mapObjDefGroupLinkList.Head(); mapObjDefGroupLink;) {
            auto next = mapObjDef->mapObjDefGroupLinkList.Next(mapObjDefGroupLink);

            CMapObjDefGroup* mapObjDefGroup = reinterpret_cast<CMapObjDefGroup*>(mapObjDefGroupLink->owner);
            CMapObjGroup* mapObjGroup = mapObjDef->owner->GetGroup(mapObjDefGroup->groupNum, true);
            if (mapObjDefGroup->bbox.t.x >= CWorld::s_objectAreaOfInterest.b.x
                && mapObjDefGroup->bbox.t.y >= CWorld::s_objectAreaOfInterest.b.y
                && mapObjDefGroup->bbox.t.z >= CWorld::s_objectAreaOfInterest.b.z
                && mapObjDefGroup->bbox.b.x <= CWorld::s_objectAreaOfInterest.t.x
                && mapObjDefGroup->bbox.b.y <= CWorld::s_objectAreaOfInterest.t.y
                && mapObjDefGroup->bbox.b.z <= CWorld::s_objectAreaOfInterest.t.z) {
                if ((mapObjGroup->unkLoadedFlag & 1) == 0) {
                    if (!mapObjGroup->asyncObjPtr)
                        mapObjDef->owner->ReadGroup(mapObjDefGroup->groupNum, false);

                    if (v23
                        && mapObjDefGroup->bbox.t.x >= CWorld::s_groupAreaOfInterest.b.x
                        && mapObjDefGroup->bbox.t.y >= CWorld::s_groupAreaOfInterest.b.y
                        && mapObjDefGroup->bbox.t.z >= CWorld::s_groupAreaOfInterest.b.z
                        && mapObjDefGroup->bbox.b.x <= CWorld::s_groupAreaOfInterest.t.x
                        && mapObjDefGroup->bbox.b.y <= CWorld::s_groupAreaOfInterest.t.y
                        && mapObjDefGroup->bbox.b.z <= CWorld::s_groupAreaOfInterest.t.z) {
                        mapObjDef->owner->WaitLoadGroup(mapObjDefGroup->groupNum);
                    }
                }

                //*(float*)&v12->unk_194 = 0.0;
                if ((mapObjGroup->unkLoadedFlag & 1) != 0) {
                    if ((mapObjDefGroup->flags & 0x10) == 0)
                        mapObjDefGroup->MarkPrepared();
                    if ((mapObjDefGroup->flags & 0x8) == 0) {
                        //CMapObj::CreateRefs(v24, v12, i, v9);
                        //CMap::FreeBaseObjLinksInBounds(&v9->bbox);
                    }
                }
            }

            dist = mapObjDefGroup->bbox.DistanceSq(CWorld::s_currentWorldPos);
            if (dist < mapObjGroup->distToCamera)
                mapObjGroup->distToCamera = dist;

            if (a1 && (mapObjDef->flags & MAPOBJ_FLAG_PREPARED) != 0) {
                if ((mapObjGroup->unkLoadedFlag & 0x1) != 0) {
                    if ((mapObjDef->flags & MAPOBJ_FLAG_DISABLED) == 0) {
                        if (CWorldScene::boundingBox.Intersects(&mapObjDefGroup->bbox)) {
                            CWorldScene::AddMapObjDefGroup(mapObjDef, mapObjDefGroup);
                        }

                        //TSExplicitList__ptr2 = v12->TSExplicitList__ptr2;
                        //if ((TSExplicitList__ptr2 & 1) == 0 && TSExplicitList__ptr2) {
                        //    v17 = TSExplicitList__ptr2;
                        //    p_mat = &i->mat;
                        //    while (1) {
                        //        C44Matrix::Translate(v20, (v17 + 4), p_mat);
                        //        C44Matrix::Translate(v21, (v17 + 16), p_mat);
                        //        CWorldView::AddOccluder(v20, v21);
                        //        v19 = *(v17 + 8);
                        //        if ((v19 & 1) != 0 || !v19)
                        //            break;
                        //        v17 = *(v17 + 8);
                        //    }
                        //}
                    }
                } else {
                    //CBarrier::AddBarrierMapObjDefGroup(&CWorldScene::s_barrier, mapObjDefGroup, 50.0);
                }
            }

			mapObjDefGroupLink = next;
		}

        //if (SLOBYTE(v1->flags) >= 0)
        //    CBarrier::AddBarrier(&CWorldScene::s_barrier, p_x, 50.0);

		mapObjDef = next;
    }
}

// OFFSET: 0x7B5D00
void CMap::PrepareMapObjDef(CMapObjDef* mapObjDef, CMapObj* mapObj) {
    mapObjDef->flags |= 0x80;
    mapObj->GetBounds(&mapObjDef->sphere);
    mapObjDef->sphere.c = mapObjDef->sphere.c * mapObjDef->mat;
    CAaBox bounds;
    mapObj->GetBounds(&bounds);
    CWorldMath::TransformAABox(mapObjDef->mat, bounds, mapObjDef->bbox);
    mapObjDef->argbColor = mapObj->argb_color;
    //ligtsCount = mapObj->ligtsCount;
    //if (ligtsCount > mapObjDef->lightsArray.m_count && ligtsCount > mapObjDef->lightsArray.m_alloc) {
    //    m_chunk = mapObjDef->lightsArray.m_chunk;
    //    if (!m_chunk)
    //        m_chunk = TSGrowableArray_4__CalcChunkSize(&mapObjDef->lightsArray.m_alloc, ligtsCount);
    //    if (ligtsCount % m_chunk)
    //        v5 = ligtsCount + m_chunk - ligtsCount % m_chunk;
    //    else
    //        v5 = ligtsCount;
    //    TSGrowableArray_CMapLight__ReallocData(&mapObjDef->lightsArray.m_alloc, v5);
    //}
    //v6 = 0;
    //mapObjDef->lightsArray.m_count = ligtsCount;
    //if (mapObjDef->lightsArray.m_count) {
    //    do
    //        *((_DWORD*)mapObjDef->lightsArray.m_data + v6++) = 0;
    //    while (v6 < mapObjDef->lightsArray.m_count);
    //}
    CMap::CreateMapObjDefGroups(mapObjDef, mapObj);
}

// OFFSET: 0x7BDE50
void CMap::CreateMapObjDefGroups(CMapObjDef* mapObjDef, CMapObj* mapObj) {
    mapObjDef->ReserveGroups(mapObj->groupInfoCount);
    for (int32_t i = 0; i < mapObj->groupInfoCount; i++) {
        CMapObjDefGroup* mapObjDefGroup = CMap::AllocMapObjDefGroup();
        CMapBaseObjLink* link = CMap::AllocBaseObjLink(mapObjDefGroup);
        link->ref = mapObjDef;
        mapObjDef->mapObjDefGroupLinkList.LinkToTail(link);
        mapObjDef->Groups()[i] = mapObjDefGroup;
        CAaBox box;
        mapObj->GetGroupBounds(&mapObjDefGroup->sphere, i);
        mapObjDefGroup->sphere.c = mapObjDefGroup->sphere.c * mapObjDef->mat;
        mapObj->GetGroupBounds(&box, i);
        CWorldMath::TransformAABox(mapObjDef->mat, box, mapObjDefGroup->bbox);
        mapObjDefGroup->groupNum = i;
        mapObjDefGroup->ambientColor = mapObjDef->argbColor;
        mapObjDefGroup->flags = 0;
        mapObjDefGroup->flags |= (mapObj->GetGroupFlags(i) & 0x48) != 0 ? 4u : 2u;
	}
}

// OFFSET: 0x7B5630
void CMap::PrepareMapDoodadDefs() {
    for (auto mapDoodadDef = CMap::doodadDefList.Head(); mapDoodadDef;) {
        auto next = CMap::doodadDefList.Next(mapDoodadDef);

        // m_next = mapDoodadDef
        CM2Model* model = mapDoodadDef->model;
        if (!model || model->IsLoaded(0, 0)) {
            if ((mapDoodadDef->unk_07C & 0x10) == 0) {
                mapDoodadDef->UpdateBounds();
                //if ((mapDoodadDef->unk_C & 2) == 0 && CMap::QueryShadow(&mapDoodadDef->position))
                //    mapDoodadDef->unk_08C = 0.5;
                //sub_7B55E0(mapDoodadDef);
                mapDoodadDef->flags |= MAPOBJ_FLAG_PREPARED | MAPOBJ_FLAG_UNPLACED;
            }
            mapDoodadDef->doodadDefLink.Unlink();
        } else {
        //    if (!SFile::IsStreamingMode() || (mapDoodadDef->unk_07C & 0x10000) == 0)
        //        goto LABEL_22;
        //    AsyncFileReadAddStreamingObject(mapDoodadDef->model, 1);
        //    mapDoodadDef->unk_07C &= ~0x10000u;
        //    mapDoodadDef = v3;
        }

        mapDoodadDef = next;
    }
}

// OFFSET: 0x7B5500
void CMap::ProcessRenderChunkUpdateList() {
    for (auto renderChunk = CMap::s_mapRenderChunkUpdateList.Head(); renderChunk;) {
        auto next = CMap::s_mapRenderChunkUpdateList.Next(renderChunk);
        renderChunk->lastUpdateTime += FrameTime::s_tickTimeSec;
        if (renderChunk->lastUpdateTime > 2.0f)
            renderChunk->FreeBuf();

        if (!renderChunk->chunkBuf)
            renderChunk->renderChunkLink.Unlink();

        renderChunk = next;
    }
}
