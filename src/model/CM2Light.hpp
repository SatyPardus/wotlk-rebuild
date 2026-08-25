#ifndef MODEL_C_M2_LIGHT_HPP
#define MODEL_C_M2_LIGHT_HPP

#include "model/M2Types.hpp"
#include <tempest/Vector.hpp>

class CM2Scene;

class CM2Light {
    public:
    // Member variables
    /* 0000 */ CM2Scene* m_scene = nullptr;
    /* 0004 */ int32_t m_stamp = 1;
    /* 0008 */ int32_t m_type = 1;
    /* 000C */ C3Vector m_pos;
    /* 0018 */ C3Vector m_viewPos;
    /* 0024 */ C3Vector m_dir;
    /* 0030 */ C3Vector m_ambColor;
    /* 003C */ C3Vector m_dirColor;
    /* 0048 */ C3Vector m_specColor;
    /* 0054 */ float m_constantAttenuation = 0.0f;
    /* 0058 */ float m_linearAttenuation = 0.69999999f;
    /* 005C */ float m_quadraticAttenuation = 0.029999999f;
    /* 0060 */ int32_t m_visible = 0;
    /* 0064 */ CM2Light** m_lightPrev = nullptr;
    /* 0068 */ CM2Light* m_lightNext = nullptr;

    // Member functions
    void Initialize(CM2Scene* scene);
    void Link();
    void SetDirection(const C3Vector& dir);
    void SetPosition(const C3Vector& dir);
    void SetLightType(M2LIGHTTYPE lightType);
    void SetVisible(int32_t visible);
    void Unlink();
};

#endif
