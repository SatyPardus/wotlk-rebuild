#ifndef GAME_UI_CAMERA_CGCAMERA_HPP
#define GAME_UI_CAMERA_CGCAMERA_HPP

#include "gameui/camera/CSimpleCamera.hpp"
#include <clientobject/WGUID.hpp>

class CGObject_C;
class CM2Model;

struct CAMERA_SMOOTH {
    int32_t startTimeMs = 0;
    float rate = 0.0f;
    float target = 0.0f;
    float startValue = 0.0f;
    float param3 = 0.0f;
    float param4 = 0.0f;
};

class CGCamera : public CSimpleCamera {
    public:
    /* 0048 */ CM2Model* m_model = nullptr;

    /* 0088 */ WGUID m_targetGUID = 0;

    /* 0098 */ uint32_t m_state = 0;
    /* 009C */ uint32_t m_flags = 0;
    /* 00A0 */ WGUID m_relativeTo = 0;

    /* 0118 */ float m_distance;
    /* 011C */ float m_yaw;
    /* 0120 */ float m_pitch;
    /* 0124 */ float m_roll;
    /* 0128 */ float m_height;

    /* 013C */ float m_flyingMountHeight;

    /* 014C */ float m_targetHeightNear = 0.0f;
    /* 0150 */ float m_targetHeightFar = 0.0f;
    /* 0154 */ float m_targetHeightSwim = 0.0f;
    /* 0158 */ float m_mountHeight = 0.0f;
    /* 015C */ int32_t m_heightChangedTimeMs = 0;

    /* 01C4 */ C3Vector m_targetPosition;
    /* 01D0 */ float unk_01D0 = 0.0f;
    /* 01D4 */ float m_targetFacing = 0.0f;

    /* 01E0 */ CAMERA_SMOOTH m_smoothDistance;
    /* 01F8 */ CAMERA_SMOOTH m_smoothGroundTilt;
    /* 0210 */ CAMERA_SMOOTH m_smoothHeight;
    /* 0228 */ CAMERA_SMOOTH m_smoothPitch;
    /* 0240 */ CAMERA_SMOOTH m_smoothTargetOffset;
    /* 0258 */ CAMERA_SMOOTH m_smoothYaw;
    /* 0270 */ CAMERA_SMOOTH m_smoothFoV;

    /* 02A8 */ CAMERA_SMOOTH m_smoothFlyingHeight;

    CGCamera();
    void SetupWorldProjection(const CRect& projectionRect);
    void UpdateCallback();
    void SetTarget(CGObject_C* target, bool a3);
    void CalcModelCamera(int32_t time);
    void CalcTargetCamera(CGObject_C* target, int32_t time);
    void CheckUnderwater();
    bool FinishLoadingTarget(CGObject_C* target);
    void UpdateTargetHeight(CGObject_C* target, int32_t time);
    bool SmoothSetHeight(float a2, float a3, float a4, int32_t time);
    bool SmoothSetFlyingMountHeight(float a2, float a3, float a4, int32_t time);
};

#endif // GAME_UI_CAMERA_CGCAMERA_HPP
