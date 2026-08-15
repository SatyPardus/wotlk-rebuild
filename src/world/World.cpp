#include "world/World.hpp"
#include "world/LoadingScreen.hpp"
#include <async/AsyncFileRead.hpp>
#include <clientobject/ObjectMgrClient.hpp>

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
    //MovementInit();
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
    namespace TriData {
        uint16_t faceIndexPool[0x4000];
        uint16_t indexPool[0xC000];
        uint32_t indexCursor;
        uint32_t faceIndexCursor;
        uint32_t statusFlags;
        uint32_t nBatches;
        Batch batches[32];
    } // namespace TriData
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
