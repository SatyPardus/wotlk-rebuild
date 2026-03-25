#include "world/map/CMap.hpp"
#include "util/SFile.hpp"
#include "world/daynight/DayNight.hpp"
#include <common/ObjectAlloc.hpp>
#include <common/Processor.hpp>
#include <cstring>
#include <storm/Error.hpp>
#include <world/CWorld.hpp>

char CMap::mapPath[STORM_MAX_PATH];
char CMap::mapName[STORM_MAX_PATH];
char CMap::wdtFilename[STORM_MAX_PATH];
uint32_t CMap::version;
SMMapHeader CMap::header;
SMAreaInfo CMap::areaInfo[64 * 64];
int32_t CMap::uniqueId;
int32_t CMap::bDungeon;
int32_t CMap::counts[11];
int32_t CMap::freeCounts[11];
TSGrowableArray<uint32_t> CMap::scCollideList;
uint32_t CMap::scCollideCnt;
uint32_t CMap::cCount;
bool CMap::bPreload;

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

void CMap::Initialize() {
    // TODO
    memset(&CMap::counts, 0, sizeof(CMap::counts));
    memset(&CMap::freeCounts, 0, sizeof(CMap::freeCounts));
    memset(&CMap::areaInfo, 0, sizeof(CMap::areaInfo));
    CMap::scCollideList.SetCount(2048);
    CMap::scCollideCnt = 0;
    CMap::cCount = 0;
    CMap::uniqueId = -2;
    CMap::bDungeon = 0;

    // TODO

    CMap::MapMemInitialize();
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
    *CMap::baseObjLinkHeap = ObjectAllocAddHeap(28, 10000, "WBASEOBJLINK", true);

    CMap::areaHeap = NEW(uint32_t);
    *CMap::areaHeap = ObjectAllocAddHeap(1212, 16, "WAREA", true);

    CMap::areaMedHeap = NEW(uint32_t);
    *CMap::areaMedHeap = ObjectAllocAddHeap(33404, 16, "WAREAMED", true);

    CMap::areaLowHeap = NEW(uint32_t);
    *CMap::areaLowHeap = ObjectAllocAddHeap(92, 16, "WAREALOW", true);

    CMap::chunkHeap = NEW(uint32_t);
    *CMap::chunkHeap = ObjectAllocAddHeap(344, 256, "WCHUNK", true);

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
    // dword_CE04A8 = sub_7D9BD0(1, 0);
    // CM2Light::SetLightType((CM2Light*)(dword_CE04A8 + 88), 0);
    // sub_7D9D50(dword_CE04A8);
    // sub_7DA100(dword_CE04A8);
    // sub_7C3730();
    // sub_7B0040(1);
    // sub_79FA10();
    // s_mapId = mapid;
    // dword_CF08F0 = 1;
    // dword_CF08F4 = 0;
    CMap::bPreload = true;
    // IsStreamingMode = SFile::IsStreamingMode();
    // dword_CE0494 = IsStreamingAndTrial() | IsStreamingMode;
    // v4 = dword_CE0494 == 0;
    // CMap::LoadWdl((int)&unk_CF0900, (char)byte_CE07D0, byte_CE06D0);
    CMap::LoadWdt();
    CMap::LoadTextureBlob();
    DayNight::LoadMap(zoneID);
    CMap::PrepareUpdate(false);
    // if (v4)
    //     AsyncFileReadWaitAll();
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

// OFFSET: 0x7B6B00
void CMap::PrepareUpdate(bool a1) {
    // if (HIDWORD(qword_CD7678))
    //     CMap::PurgeMaps();
    // CMap::bspRecurseCount = 0;
    // CMap::mapGetFacetsCount = 0;
    // CMap::oldSelectLightParm = 0;
    // sub_7CF840(flt_CD76A0);
    // CMapObj::PrepareUpdate();
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
        //        AsyncFileReadWaitAll();
        if (CWorld::s_loadProgressCallback)
            CWorld::s_loadProgressCallback(0.25, CWorld::s_loadProgressParam);
        CMap::PreUpdateAreas(a1);
        CMap::PrepareMapObjDefs(a1);
        //        CMapObj::PrepareUpdate();
        //        sub_7B5630();
        //        AsyncFileReadWaitAll();
        if (CWorld::s_loadProgressCallback)
            CWorld::s_loadProgressCallback(0.5, CWorld::s_loadProgressParam);
        CMap::PreUpdateAreas(a1);
        CMap::PrepareMapObjDefs(a1);
        //        CMapObj::PrepareUpdate();
        //        sub_7B5630();
        //        AsyncFileReadWaitAll();
        if (CWorld::s_loadProgressCallback)
            CWorld::s_loadProgressCallback(0.66000003, CWorld::s_loadProgressParam);
        CMap::PreUpdateAreas(a1);
        CMap::PrepareMapObjDefs(a1);
        //        CMapObj::PrepareUpdate();
        //        sub_7B5630();
        //        AsyncFileReadWaitAll();
        if (CWorld::s_loadProgressCallback)
            CWorld::s_loadProgressCallback(0.75, CWorld::s_loadProgressParam);
        //    }
    }
}

// OFFSET: 0x7C3730
void CMap::PurgeMaps() {
    // v0 = (unsigned int*)dword_ADFBF4;
    // if ((dword_ADFBF4 & 1) != 0 || !dword_ADFBF4)
    //     v0 = 0;
    // while (((unsigned __int8)v0 & 1) == 0 && v0) {
    //     v1 = *(unsigned int**)((char*)v0 + dword_ADFBEC + 4);
    //     v2 = (_DWORD*)v0[1];
    //     CMap::FreeBaseObjLink(v0);
    //     CMap::m_areaTable[64 * v2[19] + v2[18]] = 0;
    //     CMapArea::PurgeXXX(v2);
    //     CMap::FreeArea((int)v2);
    //     v0 = v1;
    // }
    // for (i = 0; i < 4096; ++i) {
    //     if (dword_CE08D0[i]) {
    //         sub_7C0110(dword_CE08D0[i]);
    //         dword_CE08D0[i] = 0;
    //     }
    // }
    // v4 = &dword_CF0900 + 6;
    // v5 = 4096;
    // do {
    //     if (*v4) {
    //         if (*((_DWORD*)*v4 + 21))
    //             sub_7D55B0(*((_DWORD*)*v4 + 21));
    //         *((_DWORD*)*v4 + 21) = 0;
    //     }
    //     ++v4;
    //     --v5;
    // } while (v5);
}

// OFFSET: 0x7B5950
void CMap::PreUpdateAreas(bool a1) {
    // TODO
}

// OFFSET: 0x7B6110
void CMap::PrepareMapObjDefs(bool a1) {
    // TODO
}
