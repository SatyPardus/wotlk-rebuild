#include "world/World.hpp"
#include "world/LoadingScreen.hpp"
#include <async/AsyncFileRead.hpp>
#include <clientobject/ObjectMgrClient.hpp>
#include <clientobject/Movement.hpp>
#include "world/map/CMap.hpp"

uint32_t s_newZoneID = 0;
C3Vector s_newPosition;
float s_newFacing = 0.0f;
const char* s_newMapname = nullptr;


int32_t LoadNewWorld(const void* eventData) {
    //Current = ClientServices::GetCurrent();
    //CNetClient::sub_6B1840(Current, 0);
    //MovementDestroy();
    //HIDWORD(v5) = MovementIdleMoveUnits;
    //LODWORD(v5) = EVENT_ON_IDLE;
    //EventUnregister(v5);
    //CMissile::RemoveMissiles();
    //sub_809A60();
    //CGBarberShop::DisableBarberShop();
    //ClntObjMgrDestroy();
    //sub_7FC9F0();
    //sub_6FA3C0();
    //CWorld::UnloadMap(0);
    //if (CGWorldFrame::s_currentWorldFrame)
    //    sub_4FA5D0((char*)CGWorldFrame::s_currentWorldFrame);
    //sub_6FBF00();
    //sub_783180();
    //if (dword_CD7544)
    //    sub_78D130((float*)dword_CD7544);
    //sub_4C8610(-1);
    //sub_804AF0();
    ClntObjMgrInitializeStd(s_newZoneID);
    MovementInit();
    //sub_6FAFD0();
    //sub_52CC30();
    //sub_4B9930(0, 0);
    AsyncFileReadSetProgressCallback(LoadingScreenAsyncCallback, nullptr);
    CWorld::SetLoadProgressCallback(LoadingScreenWorldCallback, nullptr);
    CWorld::LoadMap(s_newMapname, &s_newPosition, s_newZoneID);
    AsyncFileReadSetProgressCallback(nullptr, nullptr);
    CWorld::SetLoadProgressCallback(nullptr, nullptr);
    //ActiveCamera = CGWorldFrame::GetActiveCamera();
    //*(C3Vector*)(ActiveCamera + 8) = World::s_spawnPosition;
    //CSimpleCamera::SetFacing((float*)ActiveCamera, World::s_spawnRotation, 0.0, 0.0);
    //CGCamera::SetTarget(ActiveCamera, 0, 0);
    //if (a2) {
    //    v6 = off_9E0E24;
    //    v7 = 0;
    //    v8 = 0;
    //    v9[0] = 0;
    //    v9[1] = 0;
    //    v10 = -1;
    //    CDataStore::PutInt32(&v6, MSG_MOVE_WORLDPORT_ACK);
    //    v10 = 0;
    //    ClientServices::Send2(&v6);
    //    v6 = off_9E0E24;
    //    if (v9[0] != -1)
    //        CDataStore::InternalDestroy(&v7, &v8, v9);
    //}
    return 1;
}

namespace World {

    // OFFSET: 0x406DE0
    bool IsValidPosition(float x, float y, float z, float a4) {
    if (_finite(x) && _finite(y) && _finite(z)) {
        float v4 = -(y - 17066.666f);
        float v5 = -(x - 17066.666f);
        if (a4 > v4)
            return 0;
        if (34133.332f - a4 > v4 && a4 <= v5 && v5 < 34133.332f - a4)
            return 1;
    }
    return 0;
    }

    namespace TriData {
        uint16_t faceIndexPool[0x4000];
        uint16_t indexPool[0xC000];
        uint32_t indexCursor;
        uint32_t faceIndexCursor;
        uint32_t statusFlags;
        uint32_t nBatches;
        Batch batches[32];
    } // namespace TriData

    // OFFSET: 0x783910
    bool GetFacets(CAaBox* a1, CAaBox* a2, FacetData* a3, uint32_t a4, uint32_t* a5) {
        a3->facets.SetCount(0);
        a3->facetIds.SetCount(0);
        if (a5)
            *a5 = 0;
        if ((a4 & 0x4000) == 0)
            return CMap::GetFacets(a1, a2, a3, a4, a5) != 0;

        static FacetData facetData = FacetData();

        facetData.facets.SetCount(0);
        facetData.facetIds.SetCount(0);
        if (!CMap::GetFacets(a1, a2, &facetData, a4, a5))
            return 0;

        for (int32_t i = 0; i < facetData.facets.Count(); i++) {
            if (facetData.facets[i].plane.n.z >= 0.64278764f) {
                a3->facets.Add(1, &facetData.facets[i]);
                a3->facetIds.Add(1, &facetData.facetIds[i]);
            }
        }
        return 1;
    }

    // OFFSET: 0x7A3E40
    void AddAaBoxFacets(CAaBox* box, FacetData* facets) {
        static const int32_t s_boxTriIndex[36] = { 0, 2, 3, 0, 1, 2, 1, 6, 2, 1, 5, 6, 4, 6, 5, 4, 7, 6, 0, 7, 4, 0, 3, 7, 0, 5, 1, 0, 4, 5, 2, 7, 3, 2, 6, 7 };

        if (box->t.x <= box->b.x || box->t.y <= box->b.y || box->t.z <= box->b.z)
            return;

        uint32_t firstFacet = facets->facets.Count();

        C3Vector corner[8];
        corner[0] = { box->b.x, box->t.y, box->b.z };
        corner[1] = { box->t.x, box->t.y, box->b.z };
        corner[2] = { box->t.x, box->b.y, box->b.z };
        corner[3] = { box->b.x, box->b.y, box->b.z };
        corner[4] = { box->b.x, box->t.y, box->t.z };
        corner[5] = { box->t.x, box->t.y, box->t.z };
        corner[6] = { box->t.x, box->b.y, box->t.z };
        corner[7] = { box->b.x, box->b.y, box->t.z };

        C4Plane plane[6];
        plane[0].From3Pos(corner[0], corner[2], corner[3]);
        plane[1].From3Pos(corner[1], corner[6], corner[2]);
        plane[2].From3Pos(corner[4], corner[6], corner[5]);
        plane[3].From3Pos(corner[0], corner[7], corner[4]);
        plane[4].From3Pos(corner[0], corner[5], corner[1]);
        plane[5].From3Pos(corner[2], corner[7], corner[3]);

        CFacet facet(0.0f);
        for (int32_t i = 0; i < 36; i += 3) {
            facet.plane = plane[i / 6];
            facet.v[0] = corner[s_boxTriIndex[i]];
            facet.v[1] = corner[s_boxTriIndex[i + 1]];
            facet.v[2] = corner[s_boxTriIndex[i + 2]];
            facets->facets.Add(1, &facet);
        }

        uint32_t count = facets->facets.Count();
        facets->facetIds.SetCount(count);
        for (uint32_t i = firstFacet; i < count; i++)
            facets->facetIds[i] = 0;
    }

} // namespace World

World::TriData::Batch* World::TriData::AllocBatch(uint32_t indexCount, uint32_t faceCount) {
    if ((World::TriData::nBatches + 1) >= 0x20 || (indexCount + World::TriData::indexCursor) >= 0xC000 || (faceCount + World::TriData::faceIndexCursor) >= 0x4000) {
        World::TriData::statusFlags |= 1u;
        return nullptr;
    }

    World::TriData::Batch* batch = &World::TriData::batches[World::TriData::nBatches++];
    batch->matrix = nullptr;
    batch->vertexList = nullptr;
    batch->normalList = nullptr;
    batch->unk_08 = 0;
    batch->indices = nullptr;
    batch->indexCount = 0;
    batch->minVertexIndex = 0;
    batch->def = nullptr;
    batch->minVertexIndex = -1;
    return batch;
}
