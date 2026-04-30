#include "gameui/camera/CGCamera.hpp"

// OFFSET: 0x5FE880
void CGCamera::SetupWorldProjection(const CRect& projectionRect) {
    this->SetGxProjectionAndView(projectionRect);
}
