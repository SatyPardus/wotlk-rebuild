#ifndef CLIENTOBJECT_MOVEMENT_C_HPP
#define CLIENTOBJECT_MOVEMENT_C_HPP

#include "clientobject/MovementShared.hpp"
#include "clientobject/CPlayerMoveEvent.hpp"
#include <storm/List.hpp>
#include <cstdint>
#include "world/World.hpp"

class CGUnit_C;
class CClientMoveUpdate;

struct ClipPolygon {
    C3Vector v[15];
    int32_t flags[15];
    int32_t count;
};

struct MoveState {
    C3Vector anchorPos;
    float anchorFacing;
    float anchorPitch;
    uint32_t anchorElapsedMs;
    int32_t fallTimeMs;
    C3Vector moveDir;
    C2Vector moveDir2D;
    uint32_t flags;
    float currentSpeed;
};

class CMovement_C : public CMovementShared {
    public:
    // Static variables
    static STORM_EXPLICIT_LIST(CPlayerMoveEvent, m_link) s_playerMoveEventFreeList;
    static World::FacetData CMovement_C::s_moveFacets;
    static World::FacetData CMovement_C::s_liquidFacets;
    static CAaBox CMovement_C::s_queryBox;

    // Static methods
    static CPlayerMoveEvent* AllocPlayerMoveEvent(int32_t eventTime, uint32_t eventId);
    static void MoveUnits(uint32_t time, uint32_t prevTime);

    // Member variables
    /* 0000 */ //DWORD ukn1;
    /* 00C8 */ float m_collisionRadius = 0.33333334f;
    /* 00CC */ float m_collisionHeight = 2.0277777f;
    /* 00D0 */ float m_stepUpHeight = 1.0f;
    /* 00D4 */ C3Vector m_interpolationPos;
    /* 00E0 */ float m_interpolationFacing;
    /* 00E4 */ float m_interpolationPitch;
    /* 0000 */ //DWORD ukn10;
    /* 0000 */ //DWORD ukn11;
    /* 0000 */ //DWORD ukn12;
    /* 0000 */ //DWORD ukn13;
    /* 0000 */ //DWORD ukn14;
    /* 0000 */ //DWORD ukn15;
    /* 0000 */ //DWORD ukn16;
    /* 0000 */ //DWORD ukn17;
    /* 0000 */ //DWORD ukn18;
    /* 0000 */ //DWORD ukn19;
    /* 0000 */ //DWORD ukn20;
    /* 0000 */ //DWORD ukn21;
    /* 0000 */ //DWORD ukn22;
    /* 0000 */ //DWORD ukn23;
    /* 0000 */ //DWORD ukn24;
    /* 0000 */ //DWORD ukn25;
    /* 0000 */ //DWORD ukn26;
    /* 0000 */ //DWORD ukn27;
    /* 0130 */ float m_interpolation = 0.0f;
    /* 0134 */ int32_t ukn29 = 0;
    /* 0138 */ STORM_EXPLICIT_LIST(CPlayerMoveEvent, m_link) m_moveQueue;
    /* 0144 */ CGUnit_C* m_unit = nullptr;

    // Member methods
    CMovement_C() = default;
    CMovement_C(WGUID* transportGuid, C3Vector& position, float facing, WGUID* guid, CGUnit_C* unit);
    void SetUpdateInfo(int32_t time, CClientMoveUpdate* update, uint32_t a4);
    void ExecuteMovement(uint32_t time, uint32_t prevTime);
    int32_t UpdatePlayerMovement(int32_t time);
    void ApplyMovement(uint32_t a2, uint32_t a3);
    void RemoveFromMoversList(bool a2);
    bool GetCurrentHoverHeight(float* height, bool* a3, uint32_t* a4);
    void OnSplineStop(uint32_t time);
    bool IsFalling();
    bool IsValidPosition();

    void OnMoveStartLocal(int32_t eventTime, bool forward);
    void OnMoveStopLocal(int32_t eventTime);
    void OnStrafeStartLocal(int32_t eventTime, bool left);
    void OnStrafeStopLocal(int32_t eventTime);
    void OnAscendDescendStartLocal(int32_t eventTime, bool up);
    void OnAscendDescendStopLocal(int32_t eventTime);
    void OnPitchStartLocal(int32_t eventTime, bool up);
    void OnPitchStopLocal(int32_t eventTime);
    void OnTurnStartLocal(int32_t eventTime, bool left);
    void OnTurnStopLocal(int32_t eventTime);
    void AddPlayerMoveEvent(int32_t eventTime, uint32_t eventId, bool needAck, int32_t ackCounter, float facing, float pitch, uint16_t flags);
    void UnlinkMoveEventById(STORM_EXPLICIT_LIST(CPlayerMoveEvent, m_link)* list, uint32_t eventId);
    bool HasMoveEventBetween(uint32_t minEventId, uint32_t maxEventId);
    int32_t HandlePendingActions();
    int32_t RequestMove(int32_t a2, int32_t a3, C3Vector* a4);
    int32_t CollideRequestMove(int32_t a2, int32_t a3, C3Vector* a4);
    bool Interpolate(int32_t now, int32_t time, C3Vector* pos, float* facing, float* pitch);

    int32_t GetMoveStartTime(int32_t elapsed);
    float GetStepUpHeight();
    float GetRemainingStepUpHeight();
    void BuildCollisionBox(C3Vector* position, CAaBox* box);
    int32_t GetFacetQueryFlags();
    int32_t GetMoveFacets(float distance, int32_t deltaMs, float dirX, float dirY, float dirZ);
    int32_t TraceSurface(int32_t time, int32_t remaining, float distance, C2Vector* dir);
    int32_t Fall(int32_t time, int32_t remaining, float distance, C2Vector* dir);
    void UpdateFallingFar();
    int32_t Swim(int32_t time, int32_t remaining, float distance, float dirX, float dirY, float dirZ);
    int32_t HoverMove(int32_t time, int32_t remaining, float distance, float dirX, float dirY, float dirZ);
    int32_t ValidateTestVsFacetQuery(float offsetX, float offsetY, float offsetZ);
    bool IsSurfaceTooSteep(uint32_t facetId);
    bool GetBoxPushNormal(C4Plane* planes, uint32_t count, C3Vector* out);
    bool ExtrudeTriangle(C3Vector* verts, uint8_t* indices, C3Vector* normal, C4Plane* out, C3Vector* extrude);
    bool IsFacetOverhead(int32_t facet);
    bool WalkableFacetEnclosesPoint(int32_t facet, C3Vector* point);
    bool UseWalkableRedirection(int32_t facet, int32_t* detached);
    C3Vector CalcRunWalkWalkableObstaclePush(C3Vector* dir, float* stepDistance, C3Vector n, int32_t boxPush, C3Vector* pushNormal);
    C3Vector CalcRunWalkBlockingObstaclePush(C3Vector* dir, float stepDistance, float remainingDistance, C3Vector* n);
    int32_t ComputeDistanceToMoveImpl(C3Vector* verts, C3Vector* normal, C3Vector* extrude, C4Plane* out, uint8_t* indices);
    void BuildCollisionVolumePlanes(C3Vector* position, float height, float radius, C4Plane* planes);
    void BuildCollisionVolume(C3Vector* position, float height, C4Plane* planes, C3Vector* verts, uint8_t* indices);
    ClipPolygon* InitPolygonBuffer(ClipPolygon* dst, ClipPolygon* src);
    void ClipPolygonToPlane(ClipPolygon* poly, int32_t planeIndex, C4Plane* plane);
    int32_t ClipGenericPolygon(ClipPolygon* poly, C4Plane* planes, int32_t planeIndex, float* outDistance, C3Vector* direction);
    int32_t ClipMovementPyramid(World::FacetData* facets, C3Vector* direction, C4Plane* clipPlanes, uint32_t clipPlaneCount, C4Plane* volumePlanes, int32_t volumePlaneIndex, float* bestDistance, uint32_t* outFacetIndex);
    uint32_t DistanceToMovePyramid(World::FacetData* facets, C3Vector* dir, float distance, C3Vector* scaledDir, C4Plane* volumePlanes, C3Vector* verts, uint8_t* faceIndices, uint32_t* outFacetIndex, C4Plane* planes, uint32_t* outPlaneCount, float* best);
    int32_t DistanceToMove(World::FacetData* facets, C3Vector* dir, float distance, uint32_t* outFacetIndex, C4Plane* planes, uint32_t* outPlaneCount, float* outMoved, C3Vector* origin);
    int32_t TryFallingDown(float elapsedFall, C3Vector* step, float* distance, C2Vector* outSlide, WGUID* outTransport, int32_t* outLanded, int32_t* outCeiling);
    bool IsSlopeFallable(int32_t facet, C3Vector* step);
    bool CheckFallingConditions(uint32_t count, C4Plane* planes);
    int32_t FallDown(int32_t time, int32_t deltaMs, float dx, float dy, float dz, int32_t apply);
    bool CheckFallImpactThreshold(float elapsedFall, float remaining, C3Vector* step, int32_t force);
    float GetTimeJustFallen(float remaining, float elapsedFall, C3Vector* step, float* distance, int32_t force, float length2D);
    bool IsFacetSteepBothWays(int32_t facet);
    bool ComputeSteepSurfacePushNormalImpl(C4Plane* plane);
    bool PlaneIntersectsVolume(C4Plane* plane);
    void SolveThreePlaneIntersection(C4Plane* first, C4Plane* second, C4Plane* third, C3Vector* out);
    void ComputeSteepSurfacePushNormalHelper(int32_t facet, C3Vector* edge, C3Vector* origin, C3Vector* out);
    C3Vector GetSteepSurfacePushNormal(int32_t facet, C4Plane* planes, int32_t planeCount);
    C3Vector PushOffObstacleEdge(C3Vector* point, C3Vector* dir, int32_t facet);
    C2Vector CalcFallObstaclePush(C3Vector* dir, float moved, float requested, int32_t facet, C4Plane* planes, int32_t planeCount);
    void SaveMoveState(MoveState* out);
    void RestoreMoveState(MoveState* in);
    bool WillPassObstacle(C2Vector* dir2D, C3Vector* startPos, float height);
    bool AttemptStepUp(C2Vector* dir2D, C3Vector n);
};

#endif // CLIENTOBJECT_MOVEMENT_C_HPP
