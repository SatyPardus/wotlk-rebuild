#ifndef GAME_UI_CAMERA_CGCAMERA_HPP
#define GAME_UI_CAMERA_CGCAMERA_HPP

#include "gameui/camera/CSimpleCamera.hpp"

class CGCamera : public CSimpleCamera {
    public:

    void SetupWorldProjection(const CRect& projectionRect);
};

#endif // GAME_UI_CAMERA_CGCAMERA_HPP
