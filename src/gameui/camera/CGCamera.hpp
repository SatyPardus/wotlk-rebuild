#ifndef GAME_UI_CAMERA_CGCAMERA_HPP
#define GAME_UI_CAMERA_CGCAMERA_HPP

#include "gameui/camera/CSimpleCamera.hpp"
#include <clientobject/WGUID.hpp>

class CGObject_C;
class CGUnit_C;
class CM2Model;
class CGInputControl;

struct CAMERA_SMOOTH {
    int32_t startTimeMs = 0;
    float rate = 0.0f;
    float target = 0.0f;
    float startValue = 0.0f;
    float param3 = 0.0f;
    float param4 = 0.0f;
};

struct CAMERAVIEW { // 0x0C
    float distance; // +0x00
    float pitch;    // +0x04
    float yaw;      // +0x08
};

enum CAMERA_MOTION {
    CAMERA_MOTION_ZOOM_IN     = 0,
    CAMERA_MOTION_ZOOM_OUT    = 1,
    CAMERA_MOTION_YAW_LEFT    = 2,
    CAMERA_MOTION_YAW_RIGHT   = 3,
    CAMERA_MOTION_PITCH_UP    = 4,
    CAMERA_MOTION_PITCH_DOWN  = 5,
    NUM_CAMERA_MOTIONS        = 6
};

#define MAX_PITCH_ANGLE  1.55334306f
#define MIN_PITCH_ANGLE -1.55334306f
#define MAX_CAMERA_DISTANCE 50.0f

class CGCamera : public CSimpleCamera {
    public:
    static bool s_aboveFlightFloor;

    /* 0048 */ CM2Model* m_model = nullptr;

    /* 0088 */ WGUID m_targetGUID = 0;

    /* 0098 */ uint32_t m_state = 0;
    /* 009C */ uint32_t m_flags = 0;
    /* 00A0 */ WGUID m_relativeTo = 0;

    /* 00AC */ int32_t m_ignoreFacingRefs = 0;
    /* 00B0 */ int32_t unk_00B0 = 0;
    /* 00B4 */ uint32_t m_viewIndex;
    /* 00B8 */ CAMERAVIEW m_views[8];
    /* 0118 */ float m_distance;
    /* 011C */ float m_yaw;
    /* 0120 */ float m_pitch;
    /* 0124 */ float m_roll;
    /* 0128 */ float m_height;
    /* 012C */ float m_yawOffset = 0.0f;
    /* 0130 */ float m_targetOffset = 0.0f;
    /* 0134 */ float m_groundTilt = 0.0f;

    /* 013C */ float m_flyingMountHeight;

    /* 014C */ float m_targetHeightNear = 0.0f;
    /* 0150 */ float m_targetHeightFar = 0.0f;
    /* 0154 */ float m_targetHeightSwim = 0.0f;
    /* 0158 */ float m_mountHeight = 0.0f;
    /* 015C */ int32_t m_heightChangedTimeMs = 0;
    /* 0160 */ uint32_t m_motionFlags = 0;
    /* 0164 */ uint32_t m_motionTime[NUM_CAMERA_MOTIONS] = {};
    /* 017C */ uint32_t m_motionEndTime[NUM_CAMERA_MOTIONS] = {};
    /* 0194 */ uint32_t m_motionDeadline[NUM_CAMERA_MOTIONS] = {};
    /* 01AC */ float m_motionSpeed[NUM_CAMERA_MOTIONS] = {};

    /* 01C4 */ C3Vector m_targetPosition;
    /* 01D0 */ float unk_01D0 = 0.0f;
    /* 01D4 */ float m_targetFacing = 0.0f;

    /* 01DC */ float unk_01DC = 0.0f;

    /* 01E0 */ CAMERA_SMOOTH m_smoothDistance;
    /* 01F8 */ CAMERA_SMOOTH m_smoothGroundTilt;
    /* 0210 */ CAMERA_SMOOTH m_smoothHeight;
    /* 0228 */ CAMERA_SMOOTH m_smoothPitch;
    /* 0240 */ CAMERA_SMOOTH m_smoothTargetOffset;
    /* 0258 */ CAMERA_SMOOTH m_smoothYaw;
    /* 0270 */ CAMERA_SMOOTH m_smoothFoV;

    /* 02A4 */ uint32_t m_wasFlying = 0;
    /* 02A8 */ CAMERA_SMOOTH m_smoothFlyingHeight;

    /* 02C0 */ float m_collideExtent = 0.0f;
    /* 02C4 */ uint32_t m_vehicleZoomEnabled = 0;
    /* 02C8 */ float m_overrideDistanceMin = 0.0f;
    /* 02CC */ float m_overrideDistanceMax = 0.0f;

    /* 02E4 */ float unk_02E4 = 0.0f;

    /* 02FC */ int32_t unk_02FC = 0;

    CGCamera();
    void SetupWorldProjection(const CRect& projectionRect);
    void UpdateCallback();
    void SetTarget(CGObject_C* target, bool a3);
    void CalcModelCamera(int32_t time);
    void CalcTargetCamera(CGObject_C* target, int32_t time);
    void RotateOffsetByRoll(C3Vector& offset);
    void GetCameraPosition(C3Vector* out, const C3Vector& lookAt, float distance, const C3Vector& offset);
    void CheckUnderwater();
    bool FinishLoadingTarget(CGObject_C* target);
    void UpdateTargetHeight(CGObject_C* target, int32_t time);
    void UpdateTargetFacing(CGObject_C* target, float* yaw, float* pitch, float* roll);
    bool StartSmoothHeight(float height, float rate, int32_t time);
    bool SmoothSetHeight(float a2, float a3, float a4, int32_t time);
    bool StartSmoothFlyingMountHeight(float height, float rate, int32_t time);
    bool SmoothSetFlyingMountHeight(float a2, float a3, float a4, int32_t time);
    bool StartSmoothPitch(float pitch, float rate, int32_t time);
    bool SetDesiredPitchAngle(float pitch, float delay, float rateScale, int32_t time);
    bool StartSmoothYaw(float yaw, float rate, int32_t time);
    bool SetDesiredYawAngle(float yaw, float delay, float rateScale, int32_t time);
    bool StartSmoothTargetOffset(float offset, float rate, int32_t time);
    bool SetDesiredTargetOffset(float offset, float delay, float rateScale, int32_t time);
    void UpdateMotion(uint32_t time);
    void StartMotion(CAMERA_MOTION motion, uint32_t time, uint32_t duration, float speed);
    void ZoomIn(float distance, int32_t time, float duration);
    void ZoomOut(float distance, int32_t time, float duration);
    void GetSafeWorldPos(C3Vector& pos, CGObject_C* target);
    void UpdateVehicleTarget(int32_t a2);
    int32_t CanSmoothTargetFacing(CGObject_C* target);
    int32_t CanSmoothTarget();
    void IncIgnoreFacing();
    void DecIgnoreFacing();
    void ClampPitchToLimits(float delta);
    void ClampPitchAndNormalize();
    void UpdateYaw(float delta);
    void EnableFreeLook();
    void DisableFreeLook(int32_t a2);
    void SetModeFreeLook();
    void SetModeNormal();
    void SyncFreeLookFacing();
    void UpdateFreeLookFacing(float dx, float dy, float* outPitch);
    void SmoothFreeLook(CGInputControl* input, int32_t settle);
    bool CanSmoothYaw(float yawMin, float yawMax);
    bool ShouldSmoothPitch(float pitchMin, float pitchMax);
    float GetChaseFacing(CGUnit_C* target);
    bool IsCustomViewSmoothingActive();
    void CancelSmoothTargetOffset();
    void CancelSmoothYaw();
    void CancelSmoothPitch();
    void UpdateUncontrolledState(bool a2);
    void UpdateTargetSmoothing(CGObject_C* target, int32_t time);
    bool GetCameraDistance(float* distance, C3Vector* from, C3Vector* to, uint32_t flags);
    uint32_t CollideCameraWithWorld(C3Vector* target, float* distance, float* height, C3Vector* shake, float liquid, float* extent);

    // Inside CameraCVars.cpp
    bool CheckViewSmoothingCVarsChanged(uint32_t viewIndex);
};

#endif // GAME_UI_CAMERA_CGCAMERA_HPP
