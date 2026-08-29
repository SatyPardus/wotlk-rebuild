#ifndef CLIENTOBJECT_MOVEMENT_C_HPP
#define CLIENTOBJECT_MOVEMENT_C_HPP

#include "clientobject/MovementShared.hpp"
#include "clientobject/CPlayerMoveEvent.hpp"
#include <storm/List.hpp>
#include <cstdint>

class CGUnit_C;
class CClientMoveUpdate;

class CMovement_C : public CMovementShared {
    public:
    // Static variables
    static STORM_EXPLICIT_LIST(CPlayerMoveEvent, m_link) s_playerMoveEventFreeList;

    // Static methods
    static CPlayerMoveEvent* AllocPlayerMoveEvent(int32_t eventTime, uint32_t eventId);
    static void MoveUnits(uint32_t time, uint32_t prevTime);

    // Member variables
    /* 0000 */ //DWORD ukn1;
    /* 0000 */ //float ukn2;
    /* 0000 */ //float ukn3;
    /* 0000 */ //float ukn4;
    /* 0000 */ //float ukn5;
    /* 0000 */ //float ukn6;
    /* 0000 */ //float ukn7;
    /* 0000 */ //DWORD ukn8;
    /* 0000 */ //DWORD ukn9;
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
    /* 0000 */ float m_interpolation = 0.0f;
    /* 0000 */ int32_t ukn29 = 0;
    /* 0000 */ STORM_EXPLICIT_LIST(CPlayerMoveEvent, m_link) m_moveQueue;
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
    int32_t RequestMove(int32_t a2, int32_t a3, C3Vector* a4);
    bool Interpolate(int32_t now, int32_t time, C3Vector* pos, float* facing, float* pitch);

    int32_t GetMoveStartTime(int32_t elapsed);
};

#endif // CLIENTOBJECT_MOVEMENT_C_HPP
