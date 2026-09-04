#include "gameui/camera/CameraCVars.hpp"
#include "util/Unimplemented.hpp"
#include <storm/String.hpp>

CVar* s_cvCameraSavedDistance;
CVar* s_cvCameraSavedVehicleDistance;
CVar* s_cvCameraSavedPitch;
CVar* s_cvMouseInvertYaw;
CVar* s_cvMouseInvertPitch;
CVar* s_cvCameraBobbing;
CVar* s_cvCameraDistanceMoveSpeed;
CVar* s_cvCameraPitchMoveSpeed;
CVar* s_cvCameraYawMoveSpeed;
CVar* s_cvCameraBobbingSmoothSpeed;
CVar* s_cvCameraFoVSmoothSpeed;
CVar* s_cvCameraDistanceSmoothSpeed;
CVar* s_cvCameraGroundSmoothSpeed;
CVar* s_cvCameraHeightSmoothSpeed;
CVar* s_cvCameraPitchSmoothSpeed;
CVar* s_cvCameraTargetSmoothSpeed;
CVar* s_cvCameraYawSmoothSpeed;
CVar* s_cvCameraFlyingMountHeightSmoothSpeed;
CVar* s_cvCameraViewBlendStyle;
CVar* s_cvCameraView;
CVar* s_cvCameraSmooth;
CVar* s_cvCameraSmoothPitch;
CVar* s_cvCameraSmoothYaw;
CVar* s_cvCameraSmoothStyle;
CVar* s_cvCameraSmoothTrackingStyle;
CVar* s_cvCameraCustomViewSmoothing;
CVar* s_cvCameraTerrainTilt;
CVar* s_cvCameraTerrainTiltTimeMin;
CVar* s_cvCameraTerrainTiltTimeMax;
CVar* s_cvCameraWaterCollision;
CVar* s_cvCameraHeightIgnoreStandState;
CVar* s_cvCameraPivot;
CVar* s_cvCameraPivotDXMax;
CVar* s_cvCameraPivotDYMin;
CVar* s_cvCameraDive;
CVar* s_cvCameraSurfacePitch;
CVar* s_cvCameraSubmergePitch;
CVar* s_cvCameraSurfaceFinalPitch;
CVar* s_cvCameraSubmergeFinalPitch;
CVar* s_cvCameraDistanceMax;
CVar* s_cvCameraDistanceMaxFactor;
CVar* s_cvCameraPitchSmoothMin;
CVar* s_cvCameraPitchSmoothMax;
CVar* s_cvCameraYawSmoothMin;
CVar* s_cvCameraYawSmoothMax;
CVar* s_cvCameraSmoothTimeMin;
CVar* s_cvCameraSmoothTimeMax;

CVar* s_cvCameraViewSettings[NUM_CAMERA_VIEWS][NUM_CAMERA_VIEW_COMPONENTS];
CVar* s_cvCameraSmoothState[NUM_CAMERA_SMOOTH_STYLES][NUM_CAMERA_SMOOTH_STATES][NUM_CAMERA_SMOOTH_PARAMS];
CVar* s_cvCameraSmoothViewData[NUM_CAMERA_SMOOTH_STYLES][NUM_CAMERA_VIEW_COMPONENTS][NUM_CAMERA_SMOOTH_PARAMS];
CVar* s_cvCameraTerrainTiltState[NUM_CAMERA_SMOOTH_STYLES][NUM_CAMERA_TERRAIN_TILT_STATES][NUM_CAMERA_TERRAIN_TILT_PARAMS];

float s_cameraBobbingHorizontalAmplitude;
float s_cameraBobbingVerticalAmplitude;
float s_cameraBobbingSpeed;

static const char* s_cameraViewBlendStyleDefault = "1";
static const char* s_cameraViewDefault = "2";
static const char* s_cameraSmoothStyleDefault = "4";

static const char* s_cameraViewSuffix[NUM_CAMERA_VIEWS] = { "", "A", "B", "C", "D", "E", "Com", "Barber Shop" };

static const char* s_cameraViewComponentName[NUM_CAMERA_VIEW_COMPONENTS] = { "Distance", "Pitch", "Yaw" };

static const char* s_cameraViewSettingDefault[NUM_CAMERA_VIEWS][NUM_CAMERA_VIEW_COMPONENTS] = {
    { "0.0", "0.0", "0.0" },
    { "0.0", "0.0", "0.0" },
    { "5.55", "10.0", "0.0" },
    { "5.55", "20.0", "0.0" },
    { "13.88", "30.0", "0.0" },
    { "13.88", "10.0", "0.0" },
    { "0.0", "0.0", "0.0" },
    { "5.0", "10.0", "0.0" }
};

static const char* s_cameraSmoothStyleName[NUM_CAMERA_SMOOTH_STYLES] = { "Never", "Smart", "Always", "Spline", "Smarter" };

static const char* s_cameraSmoothStateName[NUM_CAMERA_SMOOTH_STATES] = { "Idle", "Stop", "Track", "Move", "Strafe", "Turn", "Fear" };

static const char* s_cameraSmoothParamName[NUM_CAMERA_SMOOTH_PARAMS] = { "Delay", "Factor" };

static const char* s_cameraSmoothStateDefault[NUM_CAMERA_SMOOTH_STYLES][NUM_CAMERA_SMOOTH_STATES][NUM_CAMERA_SMOOTH_PARAMS] = {
    { { "0.0", "0.0" }, { "0.0", "0.0" }, { "0.0", "0.0" }, { "0.0", "0.0" }, { "0.0", "0.0" }, { "0.0", "0.0" }, { "0.0", "0.0" } },
    { { "0.0", "0.0" }, { "0.0", "0.0" }, { "0.4", "10.0" }, { "0.0", "1.0" }, { "0.0", "1.0" }, { "0.0", "1.0" }, { "0.4", "10.0" } },
    { { "0.0", "1.0" }, { "0.0", "1.0" }, { "0.0", "1.0" }, { "0.0", "1.0" }, { "0.0", "1.0" }, { "0.0", "1.0" }, { "0.0", "1.0" } },
    { { "0.0", "4.0" }, { "0.0", "4.0" }, { "0.0", "4.0" }, { "0.0", "1.0" }, { "0.0", "1.0" }, { "0.0", "1.0" }, { "0.0", "4.0" } },
    { { "0.0", "0.0" }, { "0.0", "0.0" }, { "0.4", "10.0" }, { "0.0", "1.0" }, { "0.0", "1.0" }, { "0.0", "1.0" }, { "0.4", "10.0" } }
};

static const char* s_cameraSmoothViewDataDefault[NUM_CAMERA_SMOOTH_STYLES][NUM_CAMERA_VIEW_COMPONENTS][NUM_CAMERA_SMOOTH_PARAMS] = {
    { { "0.0", "0.0" }, { "0.0", "0.0" }, { "0.0", "0.0" } },
    { { "0.0", "0.0" }, { "0.0", "0.0" }, { "0.0", "1.0" } },
    { { "0.0", "0.0" }, { "0.0", "1.0" }, { "0.0", "1.0" } },
    { { "0.0", "0.0" }, { "0.0", "0.0" }, { "0.0", "1.0" } },
    { { "0.0", "1.0" }, { "0.0", "0.0" }, { "0.0", "1.0" } }
};

static const char* s_cameraTerrainTiltStateName[NUM_CAMERA_TERRAIN_TILT_STATES] = { "Fall", "Fear", "Idle", "Jump", "Move", "Strafe", "Swim", "Taxi", "Track", "Turn" };

static const char* s_cameraTerrainTiltParamName[NUM_CAMERA_TERRAIN_TILT_PARAMS] = { "Absorb", "Delay", "Factor" };

static const char* s_cameraTerrainTiltDefault[NUM_CAMERA_SMOOTH_STYLES][NUM_CAMERA_TERRAIN_TILT_STATES][NUM_CAMERA_TERRAIN_TILT_PARAMS] = {
    { { "0.0", "0.0", "-1.0" }, { "0.0", "0.0", "-1.0" }, { "0.0", "0.0", "-1.0" }, { "0.0", "0.0", "-1.0" }, { "0.0", "0.0", "-1.0" }, { "0.0", "0.0", "-1.0" }, { "0.0", "0.0", "-1.0" }, { "0.0", "0.0", "-1.0" }, { "0.0", "0.0", "-1.0" }, { "0.0", "0.0", "-1.0" } },
    { { "1.0", "0.0", "0.75" }, { "1.0", "0.0", "1.0" }, { "0.0", "0.0", "-1.0" }, { "0.0", "0.0", "-1.0" }, { "1.0", "0.0", "1.0" }, { "1.0", "0.0", "1.0" }, { "0.0", "0.0", "1.0" }, { "0.0", "0.0", "1.0" }, { "1.0", "0.0", "1.0" }, { "1.0", "0.0", "1.0" } },
    { { "1.0", "0.0", "0.75" }, { "1.0", "0.0", "1.0" }, { "1.0", "0.0", "1.0" }, { "0.0", "0.0", "-1.0" }, { "1.0", "0.0", "1.0" }, { "1.0", "0.0", "1.0" }, { "0.0", "0.0", "1.0" }, { "0.0", "0.0", "1.0" }, { "1.0", "0.0", "1.0" }, { "1.0", "0.0", "1.0" } },
    { { "1.0", "0.0", "0.75" }, { "1.0", "0.0", "1.0" }, { "0.0", "0.0", "-1.0" }, { "0.0", "0.0", "-1.0" }, { "1.0", "0.0", "1.0" }, { "1.0", "0.0", "1.0" }, { "0.0", "0.0", "1.0" }, { "0.0", "0.0", "1.0" }, { "1.0", "0.0", "1.0" }, { "1.0", "0.0", "1.0" } },
    { { "1.0", "0.0", "0.75" }, { "1.0", "0.0", "1.0" }, { "0.0", "0.0", "-1.0" }, { "0.0", "0.0", "-1.0" }, { "1.0", "0.0", "1.0" }, { "1.0", "0.0", "1.0" }, { "0.0", "0.0", "1.0" }, { "0.0", "0.0", "1.0" }, { "1.0", "0.0", "1.0" }, { "1.0", "0.0", "1.0" } }
};

static CVar::HANDLER_FUNC s_cameraViewValidator[NUM_CAMERA_VIEW_COMPONENTS] = { &ValidateCameraDistance, &ValidateCameraPitch, &ValidateCameraYaw };

// OFFSET: 0x5FD630
bool ValidateCameraView(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x5FD680
bool ValidateCameraDistance(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x5FD6D0
bool ValidateCameraPitch(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x5FD750
bool ValidateCameraTime(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x5FD7B0
bool ValidateCameraYaw(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x5FD800
bool ValidateCameraAngularSpeed(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x5FD860
bool ValidateCameraLinearSpeed(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x5FD8C0
bool ValidateCameraSmoothStyle(CVar* cvar, const char* oldValue, const char* newValue, void* arg) {
    WHOA_UNIMPLEMENTED(true);
}

// OFFSET: 0x5FD910
void CameraRegisterCVars() {
    uint32_t view = SStrToInt(s_cameraViewDefault);
    s_cvCameraSavedDistance = CVar::Register("cameraSavedDistance", nullptr, 32, s_cameraViewSettingDefault[view][0], nullptr, 5, 0, nullptr, 0);

    s_cvCameraSavedVehicleDistance = CVar::Register("cameraSavedVehicleDistance", nullptr, 32, "-1.0", nullptr, 5, 0, nullptr, 0);

    view = SStrToInt(s_cameraViewDefault);
    s_cvCameraSavedPitch = CVar::Register("cameraSavedPitch", nullptr, 32, s_cameraViewSettingDefault[view][1], nullptr, 5, 0, nullptr, 0);

    s_cvMouseInvertYaw = CVar::Register("mouseInvertYaw", nullptr, 16, "0", nullptr, 5, 0, nullptr, 0);
    s_cvMouseInvertPitch = CVar::Register("mouseInvertPitch", nullptr, 16, "0", nullptr, 5, 0, nullptr, 0);

    CVar* cameraBobbing = CVar::Register("cameraBobbing", nullptr, 0, "0", nullptr, 5, 0, nullptr, 0);

    s_cameraBobbingHorizontalAmplitude = 2.0f;
    s_cameraBobbingVerticalAmplitude = 2.0f;
    s_cameraBobbingSpeed = 0.80000001f;

    s_cvCameraBobbing = cameraBobbing;

    s_cvCameraDistanceMoveSpeed = CVar::Register("cameraDistanceMoveSpeed", nullptr, 16, "8.33", &ValidateCameraLinearSpeed, 5, 0, nullptr, 0);
    s_cvCameraPitchMoveSpeed = CVar::Register("cameraPitchMoveSpeed", nullptr, 16, "90", &ValidateCameraAngularSpeed, 5, 0, nullptr, 0);
    s_cvCameraYawMoveSpeed = CVar::Register("cameraYawMoveSpeed", nullptr, 16, "180", &ValidateCameraAngularSpeed, 5, 0, nullptr, 0);
    s_cvCameraBobbingSmoothSpeed = CVar::Register("cameraBobbingSmoothSpeed", nullptr, 16, "0.8", &ValidateCameraLinearSpeed, 5, 0, nullptr, 0);
    s_cvCameraFoVSmoothSpeed = CVar::Register("cameraFoVSmoothSpeed", nullptr, 16, "0.5", &ValidateCameraAngularSpeed, 5, 0, nullptr, 0);
    s_cvCameraDistanceSmoothSpeed = CVar::Register("cameraDistanceSmoothSpeed", nullptr, 16, "8.33", &ValidateCameraLinearSpeed, 5, 0, nullptr, 0);
    s_cvCameraGroundSmoothSpeed = CVar::Register("cameraGroundSmoothSpeed", nullptr, 16, "7.5", &ValidateCameraAngularSpeed, 5, 0, nullptr, 0);
    s_cvCameraHeightSmoothSpeed = CVar::Register("cameraHeightSmoothSpeed", nullptr, 16, "1.2", &ValidateCameraLinearSpeed, 5, 0, nullptr, 0);
    s_cvCameraPitchSmoothSpeed = CVar::Register("cameraPitchSmoothSpeed", nullptr, 16, "45", &ValidateCameraAngularSpeed, 5, 0, nullptr, 0);
    s_cvCameraTargetSmoothSpeed = CVar::Register("cameraTargetSmoothSpeed", nullptr, 16, "90", &ValidateCameraAngularSpeed, 5, 0, nullptr, 0);
    s_cvCameraYawSmoothSpeed = CVar::Register("cameraYawSmoothSpeed", nullptr, 16, "180", &ValidateCameraAngularSpeed, 5, 0, nullptr, 0);
    s_cvCameraFlyingMountHeightSmoothSpeed = CVar::Register("cameraFlyingMountHeightSmoothSpeed", nullptr, 16, "2.0", &ValidateCameraLinearSpeed, 5, 0, nullptr, 0);

    s_cvCameraViewBlendStyle = CVar::Register("cameraViewBlendStyle", nullptr, 16, s_cameraViewBlendStyleDefault, nullptr, 5, 0, nullptr, 0);
    s_cvCameraView = CVar::Register("cameraView", nullptr, 16, s_cameraViewDefault, &ValidateCameraView, 5, 0, nullptr, 0);

    for (int32_t v = 0; v < NUM_CAMERA_VIEWS; v++) {
        for (int32_t component = 0; component < NUM_CAMERA_VIEW_COMPONENTS; component++) {
            char name[64];
            name[0] = '\0';
            SStrPack(name, "camera", sizeof(name));
            SStrPack(name, s_cameraViewComponentName[component], sizeof(name));
            SStrPack(name, s_cameraViewSuffix[v], sizeof(name));

            s_cvCameraViewSettings[v][component] = CVar::Register(name, nullptr, 80, s_cameraViewSettingDefault[v][component], s_cameraViewValidator[component], 5, 0, nullptr, 0);
        }
    }

    s_cvCameraSmooth = CVar::Register("camerasmooth", nullptr, 16, "1", nullptr, 5, 0, nullptr, 0);
    s_cvCameraSmoothPitch = CVar::Register("cameraSmoothPitch", nullptr, 16, "1", nullptr, 5, 0, nullptr, 0);
    s_cvCameraSmoothYaw = CVar::Register("cameraSmoothYaw", nullptr, 16, "1", nullptr, 5, 0, nullptr, 0);
    s_cvCameraSmoothStyle = CVar::Register("cameraSmoothStyle", nullptr, 16, s_cameraSmoothStyleDefault, &ValidateCameraSmoothStyle, 5, 0, nullptr, 0);
    s_cvCameraSmoothTrackingStyle = CVar::Register("cameraSmoothTrackingStyle", nullptr, 16, s_cameraSmoothStyleDefault, &ValidateCameraSmoothStyle, 5, 0, nullptr, 0);
    s_cvCameraCustomViewSmoothing = CVar::Register("cameraCustomViewSmoothing", nullptr, 16, "0", nullptr, 5, 0, nullptr, 0);

    for (int32_t style = 0; style < NUM_CAMERA_SMOOTH_STYLES; style++) {
        for (int32_t state = 0; state < NUM_CAMERA_SMOOTH_STATES; state++) {
            for (int32_t param = 0; param < NUM_CAMERA_SMOOTH_PARAMS; param++) {
                char name[64];
                name[0] = '\0';
                SStrPack(name, "cameraSmooth", sizeof(name));
                SStrPack(name, s_cameraSmoothStyleName[style], sizeof(name));
                SStrPack(name, s_cameraSmoothStateName[state], sizeof(name));
                SStrPack(name, s_cameraSmoothParamName[param], sizeof(name));

                s_cvCameraSmoothState[style][state][param] = CVar::Register(name, nullptr, 16, s_cameraSmoothStateDefault[style][state][param], nullptr, 5, 0, nullptr, 0);
            }
        }

        for (int32_t component = 0; component < NUM_CAMERA_VIEW_COMPONENTS; component++) {
            for (int32_t param = 0; param < NUM_CAMERA_SMOOTH_PARAMS; param++) {
                char name[64];
                name[0] = '\0';
                SStrPack(name, "cameraSmoothViewData", sizeof(name));
                SStrPack(name, s_cameraSmoothStyleName[style], sizeof(name));
                SStrPack(name, s_cameraViewComponentName[component], sizeof(name));
                SStrPack(name, s_cameraSmoothParamName[param], sizeof(name));

                s_cvCameraSmoothViewData[style][component][param] = CVar::Register(name, nullptr, 16, s_cameraSmoothViewDataDefault[style][component][param], nullptr, 5, 0, nullptr, 0);
            }
        }

        for (int32_t state = 0; state < NUM_CAMERA_TERRAIN_TILT_STATES; state++) {
            for (int32_t param = 0; param < NUM_CAMERA_TERRAIN_TILT_PARAMS; param++) {
                char name[64];
                name[0] = '\0';
                SStrPack(name, "cameraTerrainTilt", sizeof(name));
                SStrPack(name, s_cameraSmoothStyleName[style], sizeof(name));
                SStrPack(name, s_cameraTerrainTiltStateName[state], sizeof(name));
                SStrPack(name, s_cameraTerrainTiltParamName[param], sizeof(name));

                s_cvCameraTerrainTiltState[style][state][param] = CVar::Register(name, nullptr, 16, s_cameraTerrainTiltDefault[style][state][param], nullptr, 5, 0, nullptr, 0);
            }
        }
    }

    s_cvCameraTerrainTilt = CVar::Register("cameraTerrainTilt", nullptr, 16, "0", nullptr, 5, 0, nullptr, 0);
    s_cvCameraTerrainTiltTimeMin = CVar::Register("cameraTerrainTiltTimeMin", nullptr, 16, "3.0", &ValidateCameraTime, 5, 0, nullptr, 0);
    s_cvCameraTerrainTiltTimeMax = CVar::Register("cameraTerrainTiltTimeMax", nullptr, 16, "10.0", &ValidateCameraTime, 5, 0, nullptr, 0);
    s_cvCameraWaterCollision = CVar::Register("cameraWaterCollision", nullptr, 16, "1", nullptr, 5, 0, nullptr, 0);
    s_cvCameraHeightIgnoreStandState = CVar::Register("cameraHeightIgnoreStandState", nullptr, 16, "0", nullptr, 5, 0, nullptr, 0);
    s_cvCameraPivot = CVar::Register("cameraPivot", nullptr, 16, "1", nullptr, 5, 0, nullptr, 0);
    s_cvCameraPivotDXMax = CVar::Register("cameraPivotDXMax", nullptr, 16, "0.05", nullptr, 5, 0, nullptr, 0);
    s_cvCameraPivotDYMin = CVar::Register("cameraPivotDYMin", nullptr, 16, "0.00", nullptr, 5, 0, nullptr, 0);
    s_cvCameraDive = CVar::Register("cameraDive", nullptr, 16, "1", nullptr, 5, 0, nullptr, 0);
    s_cvCameraSurfacePitch = CVar::Register("cameraSurfacePitch", nullptr, 16, "0.0", &ValidateCameraPitch, 5, 0, nullptr, 0);
    s_cvCameraSubmergePitch = CVar::Register("cameraSubmergePitch", nullptr, 16, "18.0", &ValidateCameraPitch, 5, 0, nullptr, 0);
    s_cvCameraSurfaceFinalPitch = CVar::Register("cameraSurfaceFinalPitch", nullptr, 16, "5.0", &ValidateCameraPitch, 5, 0, nullptr, 0);
    s_cvCameraSubmergeFinalPitch = CVar::Register("cameraSubmergeFinalPitch", nullptr, 16, "5.0", &ValidateCameraPitch, 5, 0, nullptr, 0);
    s_cvCameraDistanceMax = CVar::Register("cameraDistanceMax", nullptr, 16, "15.0", &ValidateCameraDistance, 5, 0, nullptr, 0);
    s_cvCameraDistanceMaxFactor = CVar::Register("cameraDistanceMaxFactor", nullptr, 16, "1.0", nullptr, 5, 0, nullptr, 0);
    s_cvCameraPitchSmoothMin = CVar::Register("cameraPitchSmoothMin", nullptr, 16, "0.0", &ValidateCameraPitch, 5, 0, nullptr, 0);
    s_cvCameraPitchSmoothMax = CVar::Register("cameraPitchSmoothMax", nullptr, 16, "30.0", &ValidateCameraPitch, 5, 0, nullptr, 0);
    s_cvCameraYawSmoothMin = CVar::Register("cameraYawSmoothMin", nullptr, 16, "0.0", &ValidateCameraYaw, 5, 0, nullptr, 0);
    s_cvCameraYawSmoothMax = CVar::Register("cameraYawSmoothMax", nullptr, 16, "0.0", &ValidateCameraYaw, 5, 0, nullptr, 0);
    s_cvCameraSmoothTimeMin = CVar::Register("cameraSmoothTimeMin", nullptr, 16, "0.1", &ValidateCameraTime, 5, 0, nullptr, 0);
    s_cvCameraSmoothTimeMax = CVar::Register("cameraSmoothTimeMax", nullptr, 16, "2.0", &ValidateCameraTime, 5, 0, nullptr, 0);
}
