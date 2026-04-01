#ifndef GAME_UI_CAMERA_CSIMPLECAMERA_HPP
#define GAME_UI_CAMERA_CSIMPLECAMERA_HPP

#include <tempest/vector/C3Vector.hpp>
#include <tempest/matrix/C33Matrix.hpp>

class CRect;

class CSimpleCamera {
    public:
    CSimpleCamera();
    CSimpleCamera(float nearZ, float farZ, float fov);
    virtual ~CSimpleCamera();

    C3Vector& Position() { return this->m_position; }
    C33Matrix& Facing() { return this->m_facing; }
    float NearZ() { return this->m_nearZ; }
    float FarZ() { return this->m_farZ; }
    float FOV() { return this->m_fov; }
    float Aspect() { return this->m_aspect; }

    virtual C3Vector Forward();
    virtual C3Vector Right();
    virtual C3Vector Up();

    void SetPosition(const C3Vector& position) { this->m_position = position; }
    void SetPosition(float x, float y, float z) { this->m_position = C3Vector(x, y, z); }

    void SetFacing(float yaw, float pitch, float roll);
    void SetFacing(const C3Vector& forward, const C3Vector& up);
    void SetFacing(const C3Vector& forward);

    void SetFieldOfView(float value) { this->m_fov = value; }
    void SetNearZ(float value) { this->m_nearZ = value; }
    void SetFarZ(float value) { this->m_farZ = value; }

    void SetGxProjectionAndView(const CRect& projectionRect);

    // Member variables
    C3Vector m_position;
    C33Matrix m_facing;
    float m_nearZ;
    float m_farZ;
    float m_fov;
    float m_aspect;
};

#endif // GAME_UI_CAMERA_CSIMPLECAMERA_HPP
