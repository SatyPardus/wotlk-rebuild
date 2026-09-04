#ifndef GAME_UI_CAMERA_CAMERA_CVARS_HPP
#define GAME_UI_CAMERA_CAMERA_CVARS_HPP

#include "console/CVar.hpp"
#include <cstdint>

#define NUM_CAMERA_VIEWS 8
#define NUM_CAMERA_VIEW_COMPONENTS 3
#define NUM_CAMERA_SMOOTH_STYLES 5
#define NUM_CAMERA_SMOOTH_STATES 7
#define NUM_CAMERA_SMOOTH_PARAMS 2
#define NUM_CAMERA_TERRAIN_TILT_STATES 10
#define NUM_CAMERA_TERRAIN_TILT_PARAMS 3

extern CVar* s_cvCameraSavedDistance;
extern CVar* s_cvCameraSavedVehicleDistance;
extern CVar* s_cvCameraSavedPitch;
extern CVar* s_cvMouseInvertYaw;
extern CVar* s_cvMouseInvertPitch;
extern CVar* s_cvCameraBobbing;
extern CVar* s_cvCameraDistanceMoveSpeed;
extern CVar* s_cvCameraPitchMoveSpeed;
extern CVar* s_cvCameraYawMoveSpeed;
extern CVar* s_cvCameraBobbingSmoothSpeed;
extern CVar* s_cvCameraFoVSmoothSpeed;
extern CVar* s_cvCameraDistanceSmoothSpeed;
extern CVar* s_cvCameraGroundSmoothSpeed;
extern CVar* s_cvCameraHeightSmoothSpeed;
extern CVar* s_cvCameraPitchSmoothSpeed;
extern CVar* s_cvCameraTargetSmoothSpeed;
extern CVar* s_cvCameraYawSmoothSpeed;
extern CVar* s_cvCameraFlyingMountHeightSmoothSpeed;
extern CVar* s_cvCameraViewBlendStyle;
extern CVar* s_cvCameraView;
extern CVar* s_cvCameraSmooth;
extern CVar* s_cvCameraSmoothPitch;
extern CVar* s_cvCameraSmoothYaw;
extern CVar* s_cvCameraSmoothStyle;
extern CVar* s_cvCameraSmoothTrackingStyle;
extern CVar* s_cvCameraCustomViewSmoothing;
extern CVar* s_cvCameraTerrainTilt;
extern CVar* s_cvCameraTerrainTiltTimeMin;
extern CVar* s_cvCameraTerrainTiltTimeMax;
extern CVar* s_cvCameraWaterCollision;
extern CVar* s_cvCameraHeightIgnoreStandState;
extern CVar* s_cvCameraPivot;
extern CVar* s_cvCameraPivotDXMax;
extern CVar* s_cvCameraPivotDYMin;
extern CVar* s_cvCameraDive;
extern CVar* s_cvCameraSurfacePitch;
extern CVar* s_cvCameraSubmergePitch;
extern CVar* s_cvCameraSurfaceFinalPitch;
extern CVar* s_cvCameraSubmergeFinalPitch;
extern CVar* s_cvCameraDistanceMax;
extern CVar* s_cvCameraDistanceMaxFactor;
extern CVar* s_cvCameraPitchSmoothMin;
extern CVar* s_cvCameraPitchSmoothMax;
extern CVar* s_cvCameraYawSmoothMin;
extern CVar* s_cvCameraYawSmoothMax;
extern CVar* s_cvCameraSmoothTimeMin;
extern CVar* s_cvCameraSmoothTimeMax;

extern CVar* s_cvCameraViewSettings[NUM_CAMERA_VIEWS][NUM_CAMERA_VIEW_COMPONENTS];
extern CVar* s_cvCameraSmoothState[NUM_CAMERA_SMOOTH_STYLES][NUM_CAMERA_SMOOTH_STATES][NUM_CAMERA_SMOOTH_PARAMS];
extern CVar* s_cvCameraSmoothViewData[NUM_CAMERA_SMOOTH_STYLES][NUM_CAMERA_VIEW_COMPONENTS][NUM_CAMERA_SMOOTH_PARAMS];
extern CVar* s_cvCameraTerrainTiltState[NUM_CAMERA_SMOOTH_STYLES][NUM_CAMERA_TERRAIN_TILT_STATES][NUM_CAMERA_TERRAIN_TILT_PARAMS];

extern float s_cameraBobbingHorizontalAmplitude;
extern float s_cameraBobbingVerticalAmplitude;
extern float s_cameraBobbingSpeed;

bool ValidateCameraView(CVar* cvar, const char* oldValue, const char* newValue, void* arg);
bool ValidateCameraDistance(CVar* cvar, const char* oldValue, const char* newValue, void* arg);
bool ValidateCameraPitch(CVar* cvar, const char* oldValue, const char* newValue, void* arg);
bool ValidateCameraTime(CVar* cvar, const char* oldValue, const char* newValue, void* arg);
bool ValidateCameraYaw(CVar* cvar, const char* oldValue, const char* newValue, void* arg);
bool ValidateCameraAngularSpeed(CVar* cvar, const char* oldValue, const char* newValue, void* arg);
bool ValidateCameraLinearSpeed(CVar* cvar, const char* oldValue, const char* newValue, void* arg);
bool ValidateCameraSmoothStyle(CVar* cvar, const char* oldValue, const char* newValue, void* arg);

void CameraRegisterCVars();

#endif // GAME_UI_CAMERA_CAMERA_CVARS_HPP
