#include "world/World.hpp"
#include "world/LoadingScreen.hpp"
#include <async/AsyncFileRead.hpp>
#include <clientobject/ObjectMgrClient.hpp>
#include <clientobject/Movement.hpp>
#include "world/map/CMap.hpp"
#include "world/map/CFrustum.hpp"
#include <util/Unimplemented.hpp>

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

    // OFFSET: 0x78FFA0
    void CLIPINFO::Set(const C3Vector& ndc) {
        this->d[0] = ndc.x;
        this->d[1] = 1.0f - ndc.x;
        this->d[2] = ndc.y;
        this->d[3] = 1.0f - ndc.y;
        this->d[4] = ndc.z;
        this->d[5] = 1.0f - ndc.z;

        const uint32_t* bits = reinterpret_cast<const uint32_t*>(this->d);

        this->outcode = (bits[0] & 0x80000000) + ((bits[1] >> 1) & 0x40000000) + ((bits[2] >> 2) & 0x20000000) + ((bits[3] >> 3) & 0x10000000) + ((bits[4] >> 4) & 0x08000000) + ((bits[5] >> 5) & 0x04000000);
    }

    // OFFSET: 0x791380
    bool NDCClip(C3Vector* verts, uint32_t count, C3Vector*** outVerts, uint32_t* outCount) {
        static C3Vector* s_clipVertsA[NDCCLIP_MAX];
        static C3Vector* s_clipVertsB[NDCCLIP_MAX];
        static C3Vector s_clipVertPool[NDCCLIP_MAX];

        CLIPINFO srcInfo[NDCCLIP_MAX];
        CLIPINFO genInfo[NDCCLIP_MAX];
        CLIPINFO* srcInfoPtrs[NDCCLIP_MAX];
        CLIPINFO* dstInfoPtrs[NDCCLIP_MAX];

        C3Vector* vertCursor = s_clipVertPool;
        CLIPINFO* infoCursor = genInfo;

        uint32_t allOutside = 0xFFFFFFFF;
        uint32_t anyOutside = 0;

        if (!count)
            return false;

        for (uint32_t i = 0; i < count; i++) {
            srcInfo[i].Set(verts[i]);
            srcInfoPtrs[i] = &srcInfo[i];
            allOutside &= srcInfo[i].outcode;
            anyOutside |= srcInfo[i].outcode;
            s_clipVertsB[i] = &verts[i];
        }

        if (allOutside)
            return false;

        if (!anyOutside) {
            *outVerts = s_clipVertsB;
            *outCount = count;
            return true;
        }

        CLIPPOLY polyA = { s_clipVertsB, srcInfoPtrs, count };
        CLIPPOLY polyB = { s_clipVertsA, dstInfoPtrs, 0 };

        CLIPPOLY* cur = &polyA;
        CLIPPOLY* dst = &polyB;

        uint32_t planeBit = 0x80000000;

        for (uint32_t k = 0; k < 6; k++) {
            if (planeBit & anyOutside) {
                uint32_t prev = cur->count - 1;
                uint32_t prevOut = planeBit & cur->infos[prev]->outcode;

                dst->count = 0;

                for (uint32_t i = 0; i < cur->count; i++) {
                    uint32_t curOut = planeBit & cur->infos[i]->outcode;

                    if (prevOut != curOut) {
                        float denom = cur->infos[prev]->d[k] - cur->infos[i]->d[k];

                        if (denom == 0.0f)
                            denom = 0.000099999997f;

                        float t = cur->infos[prev]->d[k] / denom;

                        C3Vector* pv = cur->verts[prev];
                        C3Vector* cv = cur->verts[i];

                        vertCursor->x = (cv->x - pv->x) * t + pv->x;
                        vertCursor->y = (cv->y - pv->y) * t + pv->y;
                        vertCursor->z = (cv->z - pv->z) * t + pv->z;

                        infoCursor->Set(*vertCursor);

                        dst->verts[dst->count] = vertCursor;
                        dst->infos[dst->count] = infoCursor;
                        dst->count++;

                        vertCursor++;
                        infoCursor++;
                    }

                    if (!curOut) {
                        dst->verts[dst->count] = cur->verts[i];
                        dst->infos[dst->count] = cur->infos[i];
                        dst->count++;
                    }

                    prev = i;
                    prevOut = curOut;
                }

                if (!dst->count)
                    return false;

                CLIPPOLY* swap = cur;
                cur = dst;
                dst = swap;
            }

            planeBit >>= 1;
        }

        *outVerts = cur->verts;
        *outCount = cur->count;

        return true;
    }

    // OFFSET: 0x77F330
    bool GetFacets(CFrustum* frustum, FacetData* facets, uint32_t flags, uint32_t* a4) {
        return CMap::QueryFacets(frustum, facets, flags, a4);
    }

    // OFFSET: 0x791640
    bool NDCXform(CFrustum* frustum, C44Matrix* out, bool includeTranslation) {
        C3Vector* corners = frustum->corners;

        float ax = corners[3].x - corners[0].x;
        float ay = corners[3].y - corners[0].y;
        float az = corners[3].z - corners[0].z;

        float bx = corners[1].x - corners[0].x;
        float by = corners[1].y - corners[0].y;
        float bz = corners[1].z - corners[0].z;

        float cx = corners[4].x - corners[0].x;
        float cy = corners[4].y - corners[0].y;
        float cz = corners[4].z - corners[0].z;

        *out = C44Matrix();

        out->a0 = ax;
        out->a1 = ay;
        out->a2 = az;
        out->b0 = bx;
        out->b1 = by;
        out->b2 = bz;
        out->c0 = cx;
        out->c1 = cy;
        out->c2 = cz;

        if (includeTranslation) {
            out->d0 = corners[0].x;
            out->d1 = corners[0].y;
            out->d2 = corners[0].z;
        }

        float det = out->Determinant();

        if (fabs(det) < 0.00000023841858f) {
            *out = C44Matrix();
            return false;
        }

        *out = out->Inverse(det);

        return true;
    }

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

    // OFFSET: 0x782740
    uint32_t TriDataToFacetData(void* unused, FacetData* facets, WGUID guid) {
        uint32_t before = facets->facets.Count();

        for (uint32_t b = 0; b < TriData::nBatches; b++) {
            TriData::Batch* batch = &TriData::batches[b];

            C44Matrix* matrix = batch->matrix;
            C3Vector* verts = batch->vertexList;
            uint16_t* indices = batch->indices;

            for (uint32_t face = 0; face < batch->faceCount; face++) {
                CFacet* facet = facets->facets.New();

                C3Vector p0 = verts[indices[0]];
                C3Vector p1 = verts[indices[1]];
                C3Vector p2 = verts[indices[2]];

                float ax = p2.x - p0.x;
                float ay = p2.y - p0.y;
                float az = p2.z - p0.z;

                float bx = p1.x - p0.x;
                float by = p1.y - p0.y;
                float bz = p1.z - p0.z;

                float cx = by * az - bz * ay;
                float cy = bz * ax - az * bx;
                float cz = bx * ay - ax * by;

                facet->plane.n.x = matrix->c0 * cz + matrix->b0 * cy + matrix->a0 * cx;
                facet->plane.n.y = matrix->c1 * cz + matrix->b1 * cy + matrix->a1 * cx;
                facet->plane.n.z = cx * matrix->a2 + (cy * matrix->b2 + cz * matrix->c2);

                facet->v[0] = matrix->TransformPoint(p0);
                facet->v[1] = matrix->TransformPoint(p1);
                facet->v[2] = matrix->TransformPoint(p2);

                float lengthSq = facet->plane.n.z * facet->plane.n.z + facet->plane.n.y * facet->plane.n.y + facet->plane.n.x * facet->plane.n.x;

                if (0.0f == lengthSq) {
                    // SysMsgPrintf_0(2, 2, "Found degenerate triangle -- data needs to be fixed\n");
                    facet->plane.n.x = 0.0f;
                    facet->plane.n.y = 0.0f;
                    facet->plane.n.z = 1.0f;
                } else {
                    float inverse = 1.0f / sqrtf(lengthSq);
                    facet->plane.n.x = inverse * facet->plane.n.x;
                    facet->plane.n.y = inverse * facet->plane.n.y;
                    facet->plane.n.z = inverse * facet->plane.n.z;
                }

                facet->plane.d = -(facet->v[0].z * facet->plane.n.z + facet->v[0].y * facet->plane.n.y + facet->v[0].x * facet->plane.n.x);

                indices += 3;
            }
        }

        uint32_t count = facets->facets.Count();
        if (count == before)
            return 0;

        facets->facetIds.SetCount(count);

        uint32_t i = before;
        for (; i < count; i++) {
            facets->facetIds[i] = (uint64_t)guid;
        }

        return i;
    }

    // OFFSET: 0x77F8D0
    int32_t GetFlightBoundsLower(const C3Vector& pos, float* height) {
        return CMap::GetFlightBounds(pos, height, 1);
    }

    // OFFSET: 0x77F310
    bool Intersect(C3Vector* start, C3Vector* end, C3Vector* hitPoint, float* distance, uint32_t flags, void* hitInfo) {
        return CMap::Intersect(start, end, hitPoint, distance, flags, hitInfo);
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
